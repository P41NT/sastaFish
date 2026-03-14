#include "../include/search.hpp"
#include "../include/board.hpp"
#include "../include/evaluation.hpp"
#include "../include/movegen.hpp"
#include "../include/ttable.hpp"
#include "../include/openingbook.hpp"
#include "../include/zobrist.hpp"

#include <algorithm>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <chrono>
#include <random>
#include <iostream>

namespace search {
    int quiesce(Board &b, TTable &tt, int alpha, int beta, int depth, bool &stopSearch, int &nodes, timestamp deadline) {

        if ((nodes & 2047) == 0) {
            if (std::chrono::high_resolution_clock::now() >= deadline) {
                stopSearch = true;
            }
        }

        nodes++;
        TTableEntry *entry = tt.getEntry(b.currState.polyglotHash);
        Move bestMove;
        if (entry != nullptr && entry->depth >= depth) {
            switch (entry->flag) {
                case EXACT:
                    return entry->score;
                case LOWERBOUND:
                    alpha = std::max(alpha, entry->score);
                    break;
                case UPPERBOUND:
                    beta = std::min(beta, entry->score);
                    break;
            }
            bestMove = entry->bestMove;
            if (alpha >= beta) return entry->score;
        }
        int standingPat = eval::evaluateBoard(b);

        if (depth == 0) return standingPat;

        if (standingPat >= beta) return beta;
        if (standingPat > alpha) alpha = standingPat;

        int originalAlpha = alpha;

        MoveList psuedoLegal{};
        moveGen::genPsuedoLegalCaptures(b, psuedoLegal);

        std::array<int, 256> scores;
        for (int i = 0; i < psuedoLegal.size(); i++) {
            if (psuedoLegal[i].isPromotion()) scores[i] += 50000;
            if (psuedoLegal[i].isCapture()) scores[i] += eval::mvvlvaScore(psuedoLegal[i], b);
        }

        static const int delta = 50;
        bool searchCut = false;
        const int psuedoLegalSize = psuedoLegal.size();
        unsigned legalMovesMade = 0;
        Color currColor = b.currState.currentPlayer;

        Move bestQMove{};
        bestQMove.move = 0x0000;

        for (int i = 0; i < psuedoLegal.size(); i++) {
            int best_index = std::max_element(scores.begin() + i, scores.begin() + psuedoLegalSize) - scores.begin();
            int bestScore = scores[best_index];

            std::swap(scores[best_index], scores[i]);
            std::swap(psuedoLegal[best_index], psuedoLegal[i]);

            if (!(psuedoLegal[i].isPromotion() || psuedoLegal[i].isCapture())) continue;

            Move move = psuedoLegal[i];

            if (move.isCapture()) {
                PieceType capturedPiece = move.isEnPassant() ? PAWN : b.board[move.to()].pieceType;
                int capturedVal = eval::pieceValues[capturedPiece];
                if (standingPat + capturedVal + delta < alpha) continue;
            }

            if (!b.legalCheck(move)) continue;

            auto prevHash = b.currState.polyglotHash;

            b.makeMove(move);
			int score = -quiesce(b, tt, -beta, -alpha, depth - 1, stopSearch, nodes, deadline);
			b.unMakeMove();
			legalMovesMade++;

            if (stopSearch) {
                searchCut = true;
                break;
            }

            if (score >= beta) return beta;
            if (score > alpha) {
                alpha = score;
                bestQMove = move;
            }
        }

        if (searchCut) return originalAlpha;

        TTableFlag flag = EXACT;
        if (alpha <= originalAlpha) flag = UPPERBOUND;

        tt.setEntry(b.currState.polyglotHash, 0, alpha, flag, bestQMove);

        return alpha;
    }

    int alphaBetaSearch(Board &b, TTable &tt, RepetitionTable &rt, int alpha, int beta, int depth, int &nodes, bool &stopSearch, timestamp deadline) { 
        nodes++;

        if ((nodes & 2047) == 0) {
            if (std::chrono::high_resolution_clock::now() >= deadline) {
                stopSearch = true;
            }
        }

        if (stopSearch) return 0;

        if (rt.getEntry(b.currState.polyglotHash) >= 2) { 
            return 0; 
        }

        auto originalHash = b.currState.polyglotHash;

        TTableEntry *entry = tt.getEntry(b.currState.polyglotHash);
        Move bestMove{};
        bestMove.move = 0x0000;

        if (entry != nullptr && entry->depth >= depth) {
            switch (entry->flag) {
                case EXACT:
                    return entry->score;
                case LOWERBOUND:
                    alpha = std::max(alpha, entry->score);
                    break;
                case UPPERBOUND:
                    beta = std::min(beta, entry->score);
                    break;
            }
            bestMove = entry->bestMove;
            if (alpha >= beta) return entry->score;
        }

        int originalAlpha = alpha;

        if (depth == 0) {
            nodes--;
            int result = quiesce(b, tt, alpha, beta, 10, stopSearch, nodes, deadline);
            if (stopSearch) 
                return originalAlpha;
            return result;
        }

        MoveList psuedoLegal;
        //moveGen::genLegalMoves(b, psuedoLegal);
        // right now no castling
        moveGen::genPsuedoLegalMoves(b, psuedoLegal);

        //if (psuedoLegal.empty()) {
        //    if (b.currState.isInCheck) return -inf;
        //    else return 0;
        //}

        bool searchCut = false;

        const Color currColor = b.currState.currentPlayer;
        unsigned int legalMovesPlayed = 0;

        std::array<int, 256> scores;
        for (int i = 0; i < psuedoLegal.size(); i++) {
            scores[i] = 0;
            if (psuedoLegal[i] == bestMove) scores[i] = 1000000;
            if (psuedoLegal[i].isPromotion()) {
                if (psuedoLegal[i].promotionPiece() == QUEEN)
                    scores[i] += 50000;
                else scores[i] -= 50000;
            }
            if (psuedoLegal[i].isCapture()) scores[i] += eval::mvvlvaScore(psuedoLegal[i], b);
        }

        const int psuedoLegalSize = psuedoLegal.size();

        for (int i = 0; i < psuedoLegal.size(); i++) {
            // selection sort
            int best_index = std::max_element(scores.begin() + i, scores.begin() + psuedoLegalSize) - scores.begin();
            int bestScore = scores[best_index];

            std::swap(scores[best_index], scores[i]);
            std::swap(psuedoLegal[best_index], psuedoLegal[i]);

            Move mv = psuedoLegal[i];

            // new legal check function not tested yet
            if (!b.legalCheck(mv)) continue;

			legalMovesPlayed++;
            b.makeMove(mv);
			rt.increment(b.currState.polyglotHash);
			int score = -alphaBetaSearch(b, tt, rt, -beta, -alpha, depth - 1, nodes, stopSearch, deadline);
			rt.decrement(b.currState.polyglotHash);
            b.unMakeMove();

            if (stopSearch) {
                searchCut = true;
                break;
            }

            if (score > alpha) {
                alpha = score;
                bestMove = mv;
            }
            if (alpha >= beta) {
                break;
            }
        }

        if (searchCut) {
            return originalAlpha;
        }

        if (legalMovesPlayed == 0) {
            if (b.currState.isInCheck) return -inf;
            else return 0;
        }

        TTableFlag flag = EXACT;
        if (alpha <= originalAlpha) flag = UPPERBOUND;
        else if (alpha >= beta) flag = LOWERBOUND;

        tt.setEntry(b.currState.polyglotHash, depth, alpha, flag, bestMove);
        return alpha;
    }

    Move bestMove(Board &b, TTable &tt, RepetitionTable &rt, book::Book& bk, int maxDepth,
            int maxTime, int &nodes, int &depth, int &score) {
        
        auto seed = static_cast<std::mt19937::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
        std::mt19937 rng(seed);

        auto entries = bk.getEntries(b.currState.polyglotHash);
        if (entries.size() != 0) {
            size_t limit = std::min<size_t>(entries.size(), 5);
            size_t nextMove = rng() % limit;
            return book::convertToMove(entries[nextMove], b);
        }
        Move result = iterativeDeepening(b, tt, rt, maxDepth, maxTime, nodes, depth, score);
        return result;
    }

    Move iterativeDeepening(Board &b, TTable &tt, RepetitionTable &rt, int maxDepth, int maxTime, int &nodes, int &depth, int &score) {

        Move bestMove;
        int bestValue = -inf;

        auto start = std::chrono::high_resolution_clock::now();
        auto deadline = start + std::chrono::milliseconds(maxTime);

        nodes = 0;

        bool stopSearch = false;
        //std::mutex mtx;
        //std::condition_variable conditionVar;

        //std::thread searchThread([&]() {
        //    std::unique_lock<std::mutex> lock(mtx);
        //    if (!conditionVar.wait_until(lock, deadline, [&]() { return stopSearch.load(); })) {
        //        stopSearch = true;
        //    }
        //});

        MoveList legalmoves;
        moveGen::genLegalMoves(b, legalmoves);

        for (depth = 1; depth <= maxDepth; depth++) {
            Move iterBestMove = bestMove;

            int alpha = -inf - 1;
            int beta = inf + 1;
            bool searchCut = false;

            if (iterBestMove.move != 0x0000) {
                b.makeMove(iterBestMove);
                rt.increment(b.currState.polyglotHash);
                int eval = -alphaBetaSearch(b, tt, rt, -beta, -alpha, depth - 1, nodes, stopSearch, deadline);
                rt.decrement(b.currState.polyglotHash);
                b.unMakeMove();

                if (stopSearch) {
                    searchCut = true;
                    break;
                }

                alpha = eval;
            }

            for (const auto &mv : legalmoves) {
                if (mv == iterBestMove) continue;

                if (mv.isPromotion() && mv.promotionPiece() != QUEEN) continue;

                b.makeMove(mv);
                rt.increment(b.currState.polyglotHash);
                int eval = -alphaBetaSearch(b, tt, rt, -beta, -alpha, depth - 1, nodes, stopSearch, deadline);
                rt.decrement(b.currState.polyglotHash);
                b.unMakeMove();


                if (stopSearch) {
                    searchCut = true;
                    break;
                }

                if (eval > alpha) {
                    alpha = eval;
                    iterBestMove = mv;
                }
            }

            if (!searchCut && !stopSearch) {
                bestValue = alpha;
                bestMove = iterBestMove;

                tt.setEntry(b.currState.polyglotHash, depth, bestValue, EXACT, bestMove);
            }
            if (stopSearch) break;
        }

        stopSearch = true;
        depth--;

        score = bestValue;
        return bestMove;
    }

}
