#include "../include/evaluation.hpp"
#include "../include/movegen.hpp"
#include "../include/bitboard.hpp"
#include <queue>

namespace eval {

    int evaluateBoard(Board &b) {

        bb minormajorPieces = b.bitboards[WHITE][KNIGHT] | b.bitboards[WHITE][BISHOP] |
                              b.bitboards[BLACK][KNIGHT] | b.bitboards[BLACK][BISHOP] |
                              b.bitboards[WHITE][ROOK] | b.bitboards[WHITE][QUEEN] |
                              b.bitboards[BLACK][ROOK] | b.bitboards[BLACK][QUEEN];


        int numPieces = bitboard::numBits(minormajorPieces);
        bool endgame = (numPieces <= 4);

        int mat = materialScore(b, endgame) * materialWeight;

        if (endgame) {
            auto mobs = mobility(b);
            int mob = mobilityWeight * (mobs.first - mobs.second);
            mat += mob;
        }

        return (b.currState.currentPlayer == WHITE ? 1 : -1) * (mat);
    }

    int materialScore(Board &b, bool endgame) {

        int score = 0;
        for (auto side : {WHITE,  BLACK}) {
            for (size_t piece = 0; piece < 6; piece++) {
                bb temp = b.bitboards[side][piece];
                while (temp) {
                    Square currSquare = bitboard::getLsbPop(temp);
                    int pieceVal = pieceValues[piece];
                    size_t p = piece;
                    if (p == 5 && endgame) p = 6;
                    pieceVal += (side == WHITE) ? pieceSquareTableWhite[p][currSquare] :
                                                  pieceSquareTableBlack[p][currSquare];
                    score += (side == WHITE ? 1 : -1) * pieceVal;
                }
            }
        }
        return score;
    }

    std::pair<int, int> mobility(Board &b) {
        Color prevCol = b.currState.currentPlayer;
        std::pair<int, int> mob;
        b.currState.currentPlayer = WHITE;
        mob.first = static_cast<int>(moveGen::genLegalMoves(b).size());

        b.currState.currentPlayer = BLACK;
        mob.second = static_cast<int>(moveGen::genLegalMoves(b).size());
        b.currState.currentPlayer = prevCol;

        return mob;
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
        size_t depth = 0;

        gain[0] = pieceValues[b.board[sq].pieceType];

        Color toMove = b.currState.currentPlayer;
        while (!attackers[toMove].empty()) {
            auto nextAttacker = attackers[toMove].front();
            attackers[toMove].pop();

            gain[depth + 1] = pieceValues[nextAttacker.first] - gain[depth];

            depth++;
            toMove = static_cast<Color>(static_cast<int>(toMove ^ 1));
        }

        for (size_t i = depth - 1; i-- > 0; ) 
            gain[i] = std::max(-gain[i + 1], gain[i]);

        return gain[0];
    }

    int mvvlvaScore(Move mv, Board &b) {
        if (mv.isCapture()) {
            PieceType piece = b.board[mv.from()].pieceType;
            PieceType capturedPiece = b.board[mv.to()].pieceType;
            return MVVLVA[capturedPiece][piece];
        }
        return 0;
    }
}
