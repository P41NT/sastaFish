#include "../include/search.hpp"
#include "../include/board.hpp"
#include "../include/evaluation.hpp"
#include "../include/movegen.hpp"
#include "../include/ttable.hpp"

#include <iostream>

#include <atomic>
#include <thread>
#include <chrono>

namespace search {
    int alphaBetaSearch(Board &b, TTable &tt, int alpha, int beta, int depth, int &nodes, 
            std::atomic<bool> &stopSearch) {

        nodes++;
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

        if (depth == 0) return quiesce(b, alpha, beta, 3, stopSearch);

        for (auto mv : moveGen::genLegalMoves(b)) {
            if (mv.isCapture()) {
                int seeVal = eval::staticExchange(b, mv.to()) - 
                    eval::pieceValues[b.board[mv.to()].pieceType];
                if (seeVal < 0) continue;
            }

            if (stopSearch) break;

            b.makeMove(mv);
            int score = -alphaBetaSearch(b, tt, -beta, -alpha, depth - 1, nodes, stopSearch);
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
    
    Move bestMove(Board &b, TTable &tt, int &nodes, int &depth, int &score) {
        Move best;
        score = -inf;

        std::atomic<bool> stopSearch = false;

        depth = 4;
        std::vector<Move> legalmoves = moveGen::genLegalMoves(b);
        for (auto mv : legalmoves) {
            b.makeMove(mv);
            int eval = -alphaBetaSearch(b, tt, -inf, inf, depth - 1, nodes, stopSearch);
            b.unMakeMove();

            if (eval > score) {
                score = eval;
                best = mv;
            }
        }
        return best;
    }

    Move iterativeDeepening(Board &b, TTable &tt, int maxDepth, int maxTime, int &nodes, int &depth, int &score) {
        Move bestMove;
        int bestValue = -inf;

        auto start = std::chrono::high_resolution_clock::now();
        auto deadline = start + std::chrono::milliseconds(maxTime);

        nodes = 0;

        std::atomic<bool> stopSearch = false;

        for (depth = 1; depth <= maxDepth; depth++) {
            bestValue = -inf;
            std::vector<Move> legalmoves = moveGen::genLegalMoves(b);

            if (bestMove.move != 0000) {
                b.makeMove(bestMove);
                int eval = -alphaBetaSearch(b, tt, -inf, inf, depth - 1, nodes, stopSearch);
                bestValue = eval;

                b.unMakeMove();
            }

            for (auto mv : legalmoves) {
                b.makeMove(mv);
                int eval = -alphaBetaSearch(b, tt, -inf, inf, depth - 1, nodes, stopSearch);
                b.unMakeMove();

                if (eval > bestValue) {
                    bestValue = eval;
                    bestMove = mv;
                }
            }
        }
        depth--;

        score = bestValue;
        return bestMove;
    }

    int quiesce(Board &b, int alpha, int beta, int depth, std::atomic<bool> &stopSearch) {
        int standingPat = eval::evaluateBoard(b);

        if (standingPat >= beta) return beta;
        if (depth == 0) return standingPat;
        if (standingPat > alpha) alpha = standingPat;

        for (auto mv : moveGen::genLegalMoves(b)) {

            if (stopSearch) break;

            if (mv.isCapture()) {
                int seeVal = eval::staticExchange(b, mv.to()) - eval::pieceValues[b.board[mv.from()].pieceType];
                if (seeVal < 0) continue;
                b.makeMove(mv);
                int score = -quiesce(b, -beta, -alpha, depth - 1, stopSearch);
                b.unMakeMove();

                if (score >= beta) {
                    return beta;
                }
                if (score > alpha) alpha = score;
            }
        }

        return alpha;
    }
}
