#include "../include/evaluation.hpp"
#include "../include/movegen.hpp"

namespace eval {
    int evaluateBoard(Board &b) {
        int mat = materialScore(b) * materialWeight;
        int mob = mobilityScore(b) * mobilityWeight;

        return mat;
    }

    int materialScore(Board &b) {
        int score = 0;
        for (auto side : {WHITE,  BLACK}) {
            for (int piece = 0; piece < 6; piece++) {
                int temp = bitboard::numBits(b.bitboards[side][piece]);
                score += ((side ^ 1) * 2 - 1) * temp * pieceValues[piece];
            }
        }
        return score;
    }

    int mobilityScore(Board &b) {
        Color prevCol = b.currState.currentPlayer;
        int score = 0;
        b.currState.currentPlayer = WHITE;
        score += moveGen::genLegalMoves(b).size();
        b.currState.currentPlayer = BLACK;
        score -= moveGen::genLegalMoves(b).size();
        b.currState.currentPlayer = prevCol;

        return score;
    }

    int quiesce(Board &b, int alpha, int beta, int depth) {
        int standingPat = evaluateBoard(b);

        if (standingPat >= beta) return beta;
        if (alpha < standingPat) alpha = standingPat;

        for (auto mv : moveGen::genLegalMoves(b)) {
            if (mv.isCapture()) {
                b.makeMove(mv);
                int score = -quiesce(b, -beta, -alpha, depth + 1);
                b.unMakeMove();

                if (score >= beta) return beta;
                if (score > alpha) alpha = score;
            }
        }

        return standingPat;
    }
}
