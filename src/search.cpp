#include "../include/search.hpp"
#include "../include/board.hpp"
#include "../include/evaluation.hpp"
#include "../include/movegen.hpp"
#include "../include/ttable.hpp"
#include "../include/openingbook.hpp"

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <chrono>
#include <random>
#include <iostream>

int count = 0;

namespace search {
    int quiesce(Board &b, int alpha, int beta, int depth, std::atomic<bool> &stopSearch) {
        int standingPat = eval::evaluateBoard(b);

        if (depth == 0) return standingPat;

        if (standingPat >= beta) return beta;
        if (standingPat > alpha) alpha = standingPat;

        int originalAlpha = alpha;

        std::vector<Move> legalMoves = moveGen::genLegalMoves(b);
        if (legalMoves.empty()) {
            if (b.currState.isInCheck) return -inf;
            else return 0;
        }

        auto legal = moveGen::genLegalMoves(b);
        std::sort(legal.begin(), legal.end(), [&b](Move x, Move y) {
            return eval::mvvlvaScore(x, b) > eval::mvvlvaScore(y, b);
        });

        static const int delta = 25;
        
        bool searchCut = false;
        for (auto mv : legal) {
            if (!mv.isCapture()) break;

            if (eval::pieceValues[b.board[mv.from()].pieceType] + standingPat + delta < alpha) continue;

            b.makeMove(mv);
            int score = -quiesce(b, -beta, -alpha, depth - 1, stopSearch);
            b.unMakeMove();

            if (stopSearch) {
                searchCut = true;
                break;
            }

            if (score > alpha) alpha = score;
            if (alpha >= beta) break;
        }

        if (searchCut) return originalAlpha;

        return alpha;
    }

    int alphaBetaSearch(Board &b, TTable &tt, RepetitionTable &rt, int alpha, int beta, int depth, 
            int &nodes, std::atomic<bool> &stopSearch) {
        nodes++;

        if (rt.getEntry(b.polyglotHash) >= 2) { 
            return 0; 
        }

        TTableEntry *entry = tt.getEntry(b.polyglotHash);
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
            if (b.currState.isInCheck) return -inf;
            else return 0;
        }

        // std::sort(legalMoves.begin(), legalMoves.end(), [&b](Move x, Move y) {
        //     return eval::mvvlvaScore(x, b) > eval::mvvlvaScore(y, b);
        // });

        bool searchCut = false;

        if (depth == 0) {
            int result = quiesce(b, alpha, beta, 10, stopSearch);
            if (stopSearch) 
                return originalAlpha;
            return result;
        }

        for (auto mv : legalMoves) {
            // if (mv.isCapture()) {
            //     int seeVal = eval::staticExchange(b, mv.to()) - 
            //         eval::pieceValues[b.board[mv.to()].pieceType];
            //     if (seeVal < 0) continue;
            // }
            
            if (mv.isPromotion() && mv.promotionPiece() != QUEEN) continue;

            b.makeMove(mv);
            rt.increment(b.polyglotHash);
            int score = -alphaBetaSearch(b, tt, rt, -beta, -alpha, depth - 1, nodes, stopSearch);
            rt.decrement(b.polyglotHash);
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

        TTableFlag flag = EXACT;
        if (alpha <= originalAlpha) flag = UPPERBOUND;
        else if (alpha >= beta) flag = LOWERBOUND;

        tt.setEntry(b.polyglotHash, depth, alpha, flag, bestMove);
        return alpha;
    }

    Move bestMove(Board &b, TTable &tt, RepetitionTable &rt, book::Book& bk, int maxDepth,
            int maxTime, int &nodes, int &depth, int &score) {
        
        std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

        auto entries = bk.getEntries(b.polyglotHash);
        if (entries.size() != 0) {
            int nextMove = rng() % std::min(5, (int)entries.size());
            return book::convertToMove(entries[nextMove], b);
        }
        Move result = iterativeDeepening(b, tt, rt, maxDepth, maxTime, nodes, depth, score);
        return result;
    }

    Move iterativeDeepening(Board &b, TTable &tt, RepetitionTable &rt, int maxDepth, int maxTime, 
            int &nodes, int &depth, int &score) {

        Move bestMove;
        int bestValue = -inf;

        auto start = std::chrono::high_resolution_clock::now();
        auto deadline = start + std::chrono::milliseconds(maxTime);

        nodes = 0;

        std::atomic<bool> stopSearch = false;
        std::mutex mtx;
        std::condition_variable conditionVar;

        std::thread searchThread([&]() {
            std::unique_lock<std::mutex> lock(mtx);
            if (!conditionVar.wait_until(lock, deadline, [&]() { return stopSearch.load(); })) {
                stopSearch = true;
            }
        });

        std::vector<Move> legalmoves = moveGen::genLegalMoves(b);

        for (depth = 1; depth <= maxDepth; depth++) {
            Move iterBestMove = bestMove;

            int alpha = -inf - 1;
            int beta = inf + 1;

            bool searchCut = false;

            if (iterBestMove.move != 0000) {
                b.makeMove(iterBestMove);
                rt.increment(b.polyglotHash);
                int eval = -alphaBetaSearch(b, tt, rt, -beta, -alpha, depth - 1, nodes, stopSearch);
                rt.decrement(b.polyglotHash);
                b.unMakeMove();

                if (stopSearch) {
                    searchCut = true;
                    break;
                }

                alpha = eval;
            }

            for (auto mv : legalmoves) {

                if (mv.isPromotion() && mv.promotionPiece() != QUEEN) continue;

                b.makeMove(mv);
                rt.increment(b.polyglotHash);

                int eval = -alphaBetaSearch(b, tt, rt, -beta, -alpha, depth - 1, nodes, stopSearch);
                rt.decrement(b.polyglotHash);
                b.unMakeMove();

                if (stopSearch) {
                    searchCut = true;
                    break;
                }

                if (eval > alpha) {
                    std::cerr << mv.getUciString() << " " << eval << std::endl;
                    alpha = eval;
                    iterBestMove = mv;
                }
            }

            if (!searchCut) {
                bestValue = alpha;
                bestMove = iterBestMove;
            }
            if (stopSearch) break;

            std::cerr << iterBestMove.getUciString() << " " << bestValue << " " << depth << std::endl;
            std::cerr << std::endl;
        }

        stopSearch = true;
        conditionVar.notify_one();
        searchThread.join();
        depth--;

        score = bestValue;
        return bestMove;
    }

}
