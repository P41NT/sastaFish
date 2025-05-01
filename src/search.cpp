#include "../include/search.hpp"
#include "../include/board.hpp"
#include "../include/evaluation.hpp"
#include "../include/movegen.hpp"
#include "../include/ttable.hpp"
#include "../include/debug.hpp"
#include "../include/openingbook.hpp"

#include <algorithm>
#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>

namespace search {
    int alphaBetaSearch(Board &b, TTable &tt, RepetitionTable &rt, int alpha, int beta, int depth, 
            int &nodes, std::atomic<bool> &stopSearch) {

        nodes++;
        if (rt.getEntry(b.zobristHash) >= 2) { return 0; }

        TTableEntry *entry = tt.getEntry(b.zobristHash);
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

        int originalAlpha = alpha;

        std::vector<Move> legalMoves = moveGen::genLegalMoves(b);
        if (legalMoves.empty()) {
            if (b.currState.isInCheck) return -inf + 1;
            else {
                return 0;
            }
        }

        if (depth == 0) return quiesce(b, alpha, beta, 0, stopSearch);

        for (auto mv : legalMoves) {
            // if (stopSearch) break;

            if (mv.isCapture()) {
                int seeVal = eval::staticExchange(b, mv.to()) - 
                    eval::pieceValues[b.board[mv.to()].pieceType];
                if (seeVal < 0) continue;
            }

            b.makeMove(mv);
            rt.increment(b.zobristHash);
            int score = -alphaBetaSearch(b, tt, rt, -beta, -alpha, depth - 1, nodes, stopSearch);
            rt.decrement(b.zobristHash);
            b.unMakeMove();

            if (score > alpha) {
                alpha = score;
                bestMove = mv;
            }
            if (alpha >= beta) {
                break;
            }
        }

        TTableFlag flag = EXACT;
        if (alpha <= originalAlpha) flag = UPPERBOUND;
        else if (alpha >= beta) flag = LOWERBOUND;

        tt.setEntry(b.zobristHash, depth, alpha, flag, bestMove);
        return alpha;
    }

    Move bestMove(Board &b, TTable &tt, RepetitionTable &rt, openingbook::Book& bk, int maxDepth,
            int maxTime, int &nodes, int &depth, int &score) {
        
        std::vector<openingbook::PolyglotEntry> entries = bk.getEntries(b.polyglotHash);
        if (entries.size() != 0) {
            int nextMove = rand() % std::min(3, (int)entries.size());
            return openingbook::convertToMove(entries[0], b);
        }
        return iterativeDeepening(b, tt, rt, maxDepth, maxTime, nodes, depth, score);
    }

    Move iterativeDeepening(Board &b, TTable &tt, RepetitionTable &rt, int maxDepth, int maxTime, 
            int &nodes, int &depth, int &score) {

        Move bestMove;
        int bestValue = -inf;


        auto start = std::chrono::high_resolution_clock::now();
        auto deadline = start + std::chrono::milliseconds(maxTime);

        debug::printBoard(b);

        nodes = 0;

        std::atomic<bool> stopSearch = false;

        std::thread searchThread([&]() {
            while (!stopSearch) {
                auto now = std::chrono::high_resolution_clock::now();
                if (now >= deadline) {
                    stopSearch = true;
                    break;
                }
            }
        });

        std::vector<Move> legalmoves = moveGen::genLegalMoves(b);

        for (depth = 1; depth <= maxDepth; depth++) {
            Move iterBestMove = bestMove;

            int alpha = -inf;
            int beta = inf;

            if (iterBestMove.move != 0000) {
                b.makeMove(iterBestMove);
                rt.increment(b.zobristHash);
                int eval = -alphaBetaSearch(b, tt, rt, -beta, -alpha, depth - 1, nodes, stopSearch);
                rt.decrement(b.zobristHash);
                b.unMakeMove();

                alpha = eval;
            }

            bool searchCut = false;

            for (auto mv : legalmoves) {

                if (mv.isCapture()) {
                    int seeVal = eval::staticExchange(b, mv.to()) - 
                        eval::pieceValues[b.board[mv.to()].pieceType];
                    if (seeVal < 0) continue;
                }

                b.makeMove(mv);
                rt.increment(b.zobristHash);
                int eval = -alphaBetaSearch(b, tt, rt, -beta, -alpha, depth - 1, nodes, stopSearch);
                rt.decrement(b.zobristHash);
                b.unMakeMove();

                std::cerr << depth << " " << mv.getUciString() << " " << eval << std::endl;

                if (stopSearch) {
                    searchCut = true;
                    break;
                }

                if (eval > alpha) {
                    alpha = eval;
                    iterBestMove = mv;
                }
            }
            std::cerr << std::endl;

            if (!searchCut) {
                bestValue = alpha;
                bestMove = iterBestMove;
            }
            if (stopSearch) break;
        }

        searchThread.join();
        depth--;

        score = bestValue;
        return bestMove;
    }

    int quiesce(Board &b, int alpha, int beta, int depth, std::atomic<bool> &stopSearch) {
        int standingPat = eval::evaluateBoard(b);

        if (standingPat >= beta) return beta;
        if (depth == 0) return standingPat;
        if (standingPat > alpha) alpha = standingPat;

        // for (auto mv : moveGen::genLegalMoves(b)) {
        //
        //     if (stopSearch) break;
        //
        //     if (mv.isCapture()) {
        //         int seeVal = eval::staticExchange(b, mv.to()) - eval::pieceValues[b.board[mv.from()].pieceType];
        //         if (seeVal < 0) continue;
        //         b.makeMove(mv);
        //         int score = -quiesce(b, -beta, -alpha, depth - 1, stopSearch);
        //         b.unMakeMove();
        //
        //         if (score >= beta) {
        //             return beta;
        //         }
        //         if (score > alpha) alpha = score;
        //     }
        // }

        return alpha;
    }
}
