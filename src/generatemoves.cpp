#include "../include/generatemoves.hpp"

namespace moveGen{
    bb knightAttackTable[64];
    bb kingAttackTable[64];
    bb diagonalMask[15];
    bb antiDiagonalMask[15];
    bb horizontalMask[8];
    bb verticalMask[8];

    bb blackPawnAttacks(bb pawns, bb opponents) { 
        return (bitboard::shiftNorthEast(pawns) | bitboard::shiftNorthWest(pawns)) & opponents;
    }
    bb whitePawnAttacks(bb pawns, bb opponents) {
        return (bitboard::shiftSouthEast(pawns) | bitboard::shiftSouthWest(pawns)) & opponents;
    }

    bb blackPawnPush(bb pawns, bb occupied) { 
        pawns = bitboard::shiftSouth(bitboard::shiftNorth(pawns) & (~occupied));
        bb doublePush = bitboard::shiftNorth(bitboard::shiftNorth(pawns & 0xff00));
        return doublePush | bitboard::shiftNorth(pawns); 
    } 
    bb whitePawnPush(bb pawns, bb occupied) { 
        pawns = bitboard::shiftNorth(bitboard::shiftSouth(pawns) & (~occupied));
        bb doublePush = bitboard::shiftSouth(bitboard::shiftSouth(pawns & 0xff000000000000));
        return doublePush | bitboard::shiftSouth(pawns); 
    } 

    void preprocessknightMoves() {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                bb currbb = bitboard::setbit(0, (Square)(i * 8 + j));
                bb attackbb = 
                    bitboard::shiftNorth(bitboard::shiftEast(bitboard::shiftEast(currbb))) |
                    bitboard::shiftNorth(bitboard::shiftNorth(bitboard::shiftEast(currbb))) |
                    bitboard::shiftNorth(bitboard::shiftNorth(bitboard::shiftWest(currbb))) |
                    bitboard::shiftNorth(bitboard::shiftWest(bitboard::shiftWest(currbb))) |
                    bitboard::shiftSouth(bitboard::shiftEast(bitboard::shiftEast(currbb))) |
                    bitboard::shiftSouth(bitboard::shiftSouth(bitboard::shiftEast(currbb))) |
                    bitboard::shiftSouth(bitboard::shiftSouth(bitboard::shiftWest(currbb))) |
                    bitboard::shiftSouth(bitboard::shiftWest(bitboard::shiftWest(currbb)));

                knightAttackTable[i * 8 + j] = attackbb;
            }
        }
    }

    void preprocesskingMoves() {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                bb currbb = bitboard::setbit(0, (Square)(i * 8 + j));
                bb attackbb = 
                    bitboard::shiftNorth(currbb) |
                    bitboard::shiftWest(currbb)  |
                    bitboard::shiftSouth(currbb) |
                    bitboard::shiftEast(currbb)  |
                    bitboard::shiftNorth((bitboard::shiftEast(currbb))) |
                    bitboard::shiftNorth((bitboard::shiftWest(currbb))) |
                    bitboard::shiftSouth((bitboard::shiftEast(currbb))) |
                    bitboard::shiftSouth((bitboard::shiftWest(currbb)));

                knightAttackTable[i * 8 + j] = attackbb;
            }
        }
    }

    void preprocessbishopMasks() {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                diagonalMask[i + j] = bitboard::setbit(diagonalMask[i + j], (Square)(i * 8 + j));
                antiDiagonalMask[7 + i - j] = 
                    bitboard::setbit(antiDiagonalMask[7 + i - j], (Square)(i * 8 + j));
            }
        }
    }

    void preprocessrookMasks() {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                horizontalMask[i] = bitboard::setbit(horizontalMask[i], (Square)(8 * i + j));
                verticalMask[j] = bitboard::setbit(verticalMask[j], (Square)(8 * i + j));
            }
        }
    }

    void init() {
        preprocesskingMoves();
        preprocessknightMoves();
        preprocessrookMasks();
        preprocessbishopMasks();
    }

    inline bb getKnightAttacks(bb friendly, Square sq) {
        return knightAttackTable[sq] & (~friendly);
    }

    inline bb getKingAttacks(bb friendly, Square sq) {
        return kingAttackTable[sq] & (~friendly);
    }

    bb genRookAttacks(bb friendly, bb occupied, Square sq) {
        bb s = 0ull;
        s = bitboard::setbit(s, sq);
        int row = sq / 8; int col = sq % 8;

        bb occupiedFlip = bitboard::flipHorizontal(occupied);
        bb sFlip = bitboard::flipHorizontal(s);
        bb sFlipV = bitboard::flipVertical(s);

        bb occupiedV = occupied & verticalMask[col];
        bb occupiedFlipV = bitboard::flipVertical(occupiedV);

        bb right = (occupied ^ (occupied - 2 * s)) & horizontalMask[row];
        bb left = bitboard::flipHorizontal((occupiedFlip ^ (occupiedFlip - 2 * sFlip)) 
                & horizontalMask[row]);

        bb bottom = (occupiedV) ^ (occupiedV - 2 * s) & verticalMask[col];

        bb top = bitboard::flipVertical(occupiedFlipV ^ (occupiedFlipV - 2 * sFlipV) 
                & verticalMask[col]);
        
        bb finalBB = right | left | top | bottom;
        bb actualFinal = finalBB & (~friendly);
        return actualFinal;
    }

    bb genBishopAttacks(bb friendly, bb occupied, Square sq) {
        bb s = 0ull;
        s = bitboard::setbit(s, sq);

        int row = sq / 8; int col = sq % 8;
        int diag = row + col;
        int anti = 7 + row - col;

        bb diagMask = diagonalMask[diag];
        bb antiMask = antiDiagonalMask[anti];

        bb occupiedD = occupied & diagMask;
        bb occupiedA = occupied & antiMask;

        bb occupiedDV = bitboard::flipVertical(occupied & diagMask);
        bb occupiedAV = bitboard::flipVertical(occupied & antiMask);

        bb sV = bitboard::flipVertical(s);

        bb bottomLeft = (occupiedD ^ (occupiedD - 2 * s)) & diagMask;
        bb bottomRight = (occupiedA ^ (occupiedA - 2 * s)) & antiMask;

        bb topRight = bitboard::flipVertical(occupiedDV ^ (occupiedDV - 2 * sV)) & diagMask;
        bb topLeft =  bitboard::flipVertical(occupiedAV ^ (occupiedAV - 2 * sV)) & antiMask;

        bb finalBitboard = bottomLeft | bottomRight | topRight | topLeft;
        bb finalBitboardRemovedFriendlies = finalBitboard & (~friendly);

        return finalBitboardRemovedFriendlies;
    }

    bb genQueenAttacks(bb friendly, bb occupied, Square sq) {
        return genBishopAttacks(friendly, occupied, sq) | genRookAttacks(friendly, occupied, sq);
    }
    
    bool isSquareAttacked(std::array<std::array<bb, 7>, 2> &boards, Square sq, Color col) {
        bb squareBB = bitboard::setbit(0ull, sq);
        bb occupied = boards[0][6] | boards[1][6];

        // bitboard::printBitboard(occupied);

        occupied &= (~squareBB);
        if (col == Color::BLACK && whitePawnAttacks(squareBB, boards[BLACK][PAWN])) return true;
        else if (col == Color::WHITE && whitePawnAttacks(squareBB, boards[WHITE][PAWN])) return true;

        if (boards[col][KING] & kingAttackTable[sq])  return true;
        if (boards[col][KNIGHT] & knightAttackTable[sq]) return true;
        if (boards[col][BISHOP] & genBishopAttacks(boards[col][6], occupied, sq)) return true;
        if (boards[col][ROOK] & genRookAttacks(boards[col][6], occupied, sq)) return true;

        return false;
    }

    void genLegalMoves(Board &board, std::vector<Board::Move> &legal) {
        Color side = board.gameState.currentPlayer;
        Color opps = board.gameState.currentPlayer;
        bb occupied = board.bitboards[0][6] | board.bitboards[1][6];
        bb temp;

        bb friendly = board.bitboards[side][6];
        bb notfriendly = board.bitboards[opps][6];

        // generate knight moves 
        temp = board.bitboards[side][KNIGHT];
        Board::Piece currPiece{KNIGHT, side};
        while (temp) {
            Square from = bitboard::getLsbPop(temp);

            bb knightAttacks = getKnightAttacks(friendly, from);
            while (knightAttacks) {
                Square to = bitboard::getLsbPop(knightAttacks);
                bool isCapture = bitboard::getBit(notfriendly, to);
                legal.push_back({currPiece, from, to, isCapture});
            }
        }
        // generate bishop moves 
        temp = board.bitboards[side][BISHOP];
        currPiece.piece = BISHOP;
        while (temp) {
            Square from = bitboard::getLsbPop(temp);
            bb bishopAttacks = genBishopAttacks(board.bitboards[side][6], occupied, from);
            while (bishopAttacks) {
                Square to = bitboard::getLsbPop(bishopAttacks);
                bb temp = bitboard::setbit(0ull, to);
                bool isCapture = bitboard::getBit(board.bitboards[side ^ 1][6], to);
                legal.push_back({currPiece, from, to, isCapture});
            }
        }
        // generate rook moves
        temp = board.bitboards[side][ROOK];
        currPiece.piece = ROOK;
        while (temp) {
            Square from = bitboard::getLsbPop(temp);
            bb rookAttacks = genRookAttacks(board.bitboards[side][6], occupied, from);
            while (rookAttacks) {
                Square to = bitboard::getLsbPop(rookAttacks);
                bool isCapture = bitboard::getBit(board.bitboards[side ^ 1][6], to);
                legal.push_back({currPiece, from, to, isCapture});
            }
        }
        // generate queen moves
        temp = board.bitboards[side][QUEEN];
        currPiece.piece = QUEEN;
        while (temp) {
            Square from = bitboard::getLsbPop(temp);
            bb queenAttacks = genQueenAttacks(board.bitboards[side][6], occupied, from);
            while (queenAttacks) {
                Square to = bitboard::getLsbPop(queenAttacks);
                bool isCapture = bitboard::getBit(board.bitboards[side ^ 1][6], to);
                legal.push_back({currPiece, from, to, isCapture});
            }
        }
        // generate king moves 
        temp = board.bitboards[side][KING];
        currPiece.piece = KING;
        while (temp) {
            Square from = bitboard::getLsbPop(temp);
            bb kingAttacks = getKingAttacks(board.bitboards[side][6], from);
            while (kingAttacks) {
                Square to = bitboard::getLsbPop(kingAttacks);
                bool isCapture = bitboard::getBit(board.bitboards[side ^ 1][6], to);
                legal.push_back({currPiece, from, to, isCapture, false});
            }
        }
        // generate pawn moves
        currPiece.piece = PAWN;
        if (side == WHITE) {
            temp = board.bitboards[WHITE][PAWN];
            while (temp) {
                Square from = bitboard::getLsbPop(temp);
                bb pawnMask = bitboard::setbit(0ull, from);
                bb pawnPush = whitePawnPush(pawnMask, occupied);
                while (pawnPush) {
                    Square to = bitboard::getLsbPop(pawnPush);
                    legal.push_back({currPiece, from, to, false, to >= A8 && to <= H8 });
                }
                bb pawnAttack = whitePawnAttacks(pawnMask, board.bitboards[BLACK][6]);
                while (pawnAttack) {
                    Square to = bitboard::getLsbPop(pawnAttack);
                    legal.push_back({currPiece, from, to, true, to >= A8 && to <= H8});
                }
            }
        }
        else {
            temp = board.bitboards[BLACK][PAWN];
            while (temp) {
                Square from = bitboard::getLsbPop(temp);
                bb pawnMask = bitboard::setbit(0ull, from);
                bb pawnPush = blackPawnPush(pawnMask, occupied);
                while (pawnPush) {
                    Square to = bitboard::getLsbPop(pawnPush);
                    legal.push_back({currPiece, from, to, false, to >= A8 && to <= H8 });
                }
                bb pawnAttack = blackPawnAttacks(pawnMask, board.bitboards[BLACK][6]);
                while (pawnAttack) {
                    Square to = bitboard::getLsbPop(pawnAttack);
                    legal.push_back({currPiece, from, to, true, to >= A8 && to <= H8});
                }
            }
        }
    }
}
