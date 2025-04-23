#include "../include/evaluation.hpp"
#include "../include/movegen.hpp"
#include "../include/bitboard.hpp"
#include <queue>

namespace eval {
    int evaluateBoard(Board &b) {
        int mat = materialScore(b) * materialWeight;
        auto mobs = mobility(b);
        int mob = mobs.first - mobs.second;

        if (b.currState.currentPlayer == WHITE) {
            if (mobs.first == 0 && b.currState.isInCheck) return -inf;
            else if (mobs.first == 0) return 0;
        }

        if (b.currState.currentPlayer == BLACK) {
            if (mobs.second == 1 && b.currState.isInCheck) return -inf;
            else if (mobs.second == 1) return 0;
        }

        return (b.currState.currentPlayer == WHITE ? 1 : -1) * mat + mob;
    }

    int materialScore(Board &b) {
        int score = 0;
        for (auto side : {WHITE,  BLACK}) {
            for (int piece = 0; piece < 6; piece++) {
                bb temp = b.bitboards[side][piece];

                while (temp) {
                    Square currSquare = bitboard::getLsbPop(temp);
                    int pieceVal = pieceValues[piece];
                    pieceVal += (side == WHITE) ? pieceSquareTableWhite[piece][currSquare] :
                                                  pieceSquareTableBlack[piece][currSquare];
                    score += (side == WHITE ? 1 : -1) * pieceValues[piece];
                }
            }
        }
        return score;
    }

    std::pair<int, int> mobility(Board &b) {
        Color prevCol = b.currState.currentPlayer;
        std::pair<int, int> mob;
        b.currState.currentPlayer = WHITE;
        mob.first += moveGen::genLegalMoves(b).size();
        b.currState.currentPlayer = BLACK;
        mob.second -= moveGen::genLegalMoves(b).size();
        b.currState.currentPlayer = prevCol;

        return mob;
    }

    int quiesce(Board &b, int alpha, int beta, int depth, int &nodes) {
        int standingPat = evaluateBoard(b);

        if (standingPat >= beta) {
            nodes++;
            return beta;
        }

        if (depth == 0) {
            nodes++;
            return standingPat;
        }

        if (standingPat > alpha) alpha = standingPat;

        for (auto mv : moveGen::genLegalMoves(b)) {
            if (mv.isCapture()) {
                int seeVal = staticExchange(b, mv.to()) - pieceValues[b.board[mv.from()].pieceType];
                if (seeVal < 0) continue;
                b.makeMove(mv);
                int score = -quiesce(b, -beta, -alpha, depth - 1, nodes);
                b.unMakeMove();

                if (score >= beta) {
                    nodes++;
                    return beta;
                }
                if (score > alpha) alpha = score;
            }
        }

        nodes++;
        return alpha;
    }

    int staticExchange(Board &b, Square sq) {
        std::array<std::queue<std::pair<PieceType, Square>>, 2> attackers;

        bb attackersBBWhite = moveGen::getAttackerbb(b.bitboards, sq, WHITE);
        bb attackersBBBlack = moveGen::getAttackerbb(b.bitboards, sq, BLACK);

        for (auto p : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING}) {
            bb currWhite = attackersBBWhite & b.bitboards[WHITE][p];
            bb currBlack = attackersBBBlack & b.bitboards[BLACK][p];

            while (currWhite) {
                Square temp = bitboard::getLsbPop(currWhite);
                attackers[WHITE].push({p, temp});
            }
            while (currBlack) {
                Square temp = bitboard::getLsbPop(currBlack);
                attackers[BLACK].push({p, temp});
            }
        }

        std::array<int, 32> gain;
        int depth = 0;

        gain[0] = pieceValues[b.board[sq].pieceType];

        Color toMove = b.currState.currentPlayer;
        while (!attackers[toMove].empty()) {
            auto nextAttacker = attackers[toMove].front();
            attackers[toMove].pop();

            gain[depth + 1] = pieceValues[nextAttacker.first] - gain[depth];

            depth++;
            toMove = static_cast<Color>(static_cast<int>(toMove ^ 1));
        }

        for (int i = depth - 1; i >= 0; i--) 
            gain[i] = std::max(-gain[i + 1], gain[i]);

        return gain[0];
    }
}
