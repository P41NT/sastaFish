#include "../include/search.hpp"
#include "../include/board.hpp"
#include "../include/evaluation.hpp"
#include "../include/movegen.hpp"
#include "../include/debug.hpp"
#include <iostream>

namespace search {
    int minimax(Board &b, int depth) {
        if (depth == 0) {
            return eval::quiesce(b, -inf, inf, 0);
        }
        int bestValue = -inf;
        for (auto mv : moveGen::genLegalMoves(b)) {

            if (mv.isCapture()) {
                int seeVal = eval::staticExchange(b, mv.to()) - 
                    eval::pieceValues[b.board[mv.to()].pieceType];
                if (seeVal < 0) continue;
            }

            b.makeMove(mv);
            int score = -minimax(b, depth - 1);
            b.unMakeMove();
            bestValue = std::max(bestValue, score);
        }
        return bestValue;
    }

    int alphaBetaSearch(Board &b, int alpha, int beta, int depth) {
        if (depth == 0) {
            return eval::evaluateBoard(b);
            // return eval::quiesce(b, -inf, inf, 0);
        }
        int bestValue = -inf;
        for (auto mv : moveGen::genLegalMoves(b)) {

            if (mv.isCapture()) {
                int seeVal = eval::staticExchange(b, mv.to()) - 
                    eval::pieceValues[b.board[mv.to()].pieceType];
                if (seeVal < 0) continue;
            }

            b.makeMove(mv);
            int score = -alphaBetaSearch(b, -beta, -alpha, depth - 1);
            b.unMakeMove();
            if (score > bestValue) {
                bestValue = score;
                if (score > alpha) alpha = score;
            }
            if (score >= beta) return bestValue;
        }
        return bestValue;
    }
    
    Move bestMove(Board &b) {
        Move best;
        int bestValue = -inf;

        int depth = 5;

        std::vector<Move> legalmoves = moveGen::genLegalMoves(b);
        for (auto mv : legalmoves) {
            b.makeMove(mv);
            int eval = -alphaBetaSearch(b, -inf, inf, depth - 1);
            b.unMakeMove();
            if (eval > bestValue) {
                bestValue = eval;
                best = mv;
            }
            std::cerr << mv.getUciString() << " " << eval << std::endl;
        }
        return best;
    }
}
