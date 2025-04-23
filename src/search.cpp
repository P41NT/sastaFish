#include "../include/search.hpp"
#include "../include/board.hpp"
#include "../include/evaluation.hpp"
#include "../include/movegen.hpp"
#include "../include/ttable.hpp"

#include <chrono>

namespace search {
    int alphaBetaSearch(Board &b, TTable &tt, int alpha, int beta, int depth, int &nodes) {
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

        if (depth == 0) 
            return eval::quiesce(b, alpha, beta, 2, nodes);

        for (auto mv : moveGen::genLegalMoves(b)) {
            if (mv.isCapture()) {
                int seeVal = eval::staticExchange(b, mv.to()) - 
                    eval::pieceValues[b.board[mv.to()].pieceType];
                if (seeVal < 0) continue;
            }

            b.makeMove(mv);
            int score = -alphaBetaSearch(b, tt, -beta, -alpha, depth - 1, nodes);
            b.unMakeMove();

            if (score > alpha) {
                alpha = score;
                bestMove = mv;
            }
            if (alpha >= beta) {
                // nodes++;
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

        depth = 4;
        std::vector<Move> legalmoves = moveGen::genLegalMoves(b);
        for (auto mv : legalmoves) {
            b.makeMove(mv);
            int eval = -alphaBetaSearch(b, tt, -inf, inf, depth - 1, nodes);
            b.unMakeMove();

            if (eval > score) {
                score = eval;
                best = mv;
            }
        }
        return best;
    }

    Move iterativeDeepening(Board &b, TTable &tt, int maxDepth, int maxTime, 
           int &nodes, int &depth, int &score) {
        Move bestMove;
        int bestValue = -inf;

        auto start = std::chrono::high_resolution_clock::now();

        nodes = 0;

        for (depth = 1; depth <= maxDepth; depth++) {
            bestValue = -inf;
            std::vector<Move> legalmoves = moveGen::genLegalMoves(b);

            if (bestMove.move != 0000) {
                b.makeMove(bestMove);
                int eval = -alphaBetaSearch(b, tt, -inf, inf, depth - 1, nodes);
                bestValue = eval;
                b.unMakeMove();
            }

            for (auto mv : legalmoves) {
                b.makeMove(mv);
                int eval = -alphaBetaSearch(b, tt, -inf, inf, depth - 1, nodes);
                b.unMakeMove();
                if (eval > bestValue) {
                    bestValue = eval;
                    bestMove = mv;
                }
            }
            auto end = std::chrono::high_resolution_clock::now();
            int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            if (elapsed > maxTime) break;
        }

        score = bestValue;
        return bestMove;
    }
}
