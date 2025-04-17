#include "../include/search.hpp"
#include "../include/board.hpp"
#include "../include/evaluation.hpp"
#include "../include/movegen.hpp"

namespace search {
    int alphaBetaSearch(Board &b, int alpha, int beta, int depth) {
        if (depth == 0) {
            return eval::quiesce(b, alpha, beta, 0);
        }
        int bestValue = -inf;
        for (auto mv : moveGen::genLegalMoves(b)) {
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
    
    Move bestMove(Board &b, int depth) {
        Move best;
        int bestValue = (b.currState.currentPlayer == WHITE) ? -inf : inf;

        std::vector<Move> legalmoves = moveGen::genLegalMoves(b);
        for (auto mv : legalmoves) {
            b.makeMove(mv);
            int eval = -alphaBetaSearch(b, -inf, inf, depth - 1);
            b.unMakeMove();
            if (b.currState.currentPlayer == WHITE && eval > bestValue) {
                bestValue = eval;
                best = mv;
            }
            else if (b.currState.currentPlayer == BLACK && eval < bestValue) {
                bestValue = eval;
                best = mv;
            }
        }
        return best;
    }
}
