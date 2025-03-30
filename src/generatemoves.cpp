#include "../include/generatemoves.hpp"
#include <iostream>

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

    bb genBishopMask(bb occupied, Square sq) {
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

        return bottomLeft | bottomRight | topRight | topLeft;
    }

    bb genRookMask(bb occupied, Square sq) {
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
        
        return top | bottom | left | right;
    }

    bool isSquareAttacked(std::array<std::array<bb, 7>, 2> &boards, Square sq, Color col) {
        bb squareBB = bitboard::setbit(0ull, sq);
        bb occupied = boards[0][6] | boards[1][6];

        // bitboard::printBitboard(occupied);

        occupied &= (~squareBB);
        if (col == Color::BLACK && whitePawnAttacks(squareBB, boards[BLACK][PAWN])) return true;
        else if (col == Color::WHITE && whitePawnAttacks(squareBB, boards[WHITE][PAWN])) return true;

        bb bishopMask = genBishopMask(occupied, sq) & (~boards[col][6]);
        bb rookMask = genRookMask(occupied, sq) & (~boards[col][6]);

        if (boards[col][KING] & kingAttackTable[sq])  return true;
        if (boards[col][KNIGHT] & knightAttackTable[sq]) return true;
        if (boards[col][BISHOP] & bishopMask) return true;
        if (boards[col][ROOK] & rookMask) return true;

        return false;
    }

    int countSquareAttacked(std::array<std::array<bb, 7>, 2> &boards, Square sq, Color col) {
        bb squareBB = bitboard::setbit(0ull, sq);
        bb occupied = boards[0][6] | boards[1][6];

        // bitboard::printBitboard(occupied);

        occupied &= (~squareBB);
        if (col == Color::BLACK && whitePawnAttacks(squareBB, boards[BLACK][PAWN])) return true;
        else if (col == Color::WHITE && whitePawnAttacks(squareBB, boards[WHITE][PAWN])) return true;

        bb bishopMask = genBishopMask(occupied, sq) & (~boards[col][6]);
        bb rookMask = genRookMask(occupied, sq) & (~boards[col][6]);

        int answer = 0;
        if (boards[col][KING] & kingAttackTable[sq])  answer++;
        if (boards[col][KNIGHT] & knightAttackTable[sq]) answer++;
        if (boards[col][BISHOP] & bishopMask) answer++;
        if (boards[col][ROOK] & rookMask) answer++;

        return answer;
    }

    void genLegalMoves(Board &board, std::vector<Board::Move> &psuedoLegal, std::vector<Board::Move> &legal) {
        bool isInCheck = board.gameState.isInCheck;
        for (auto &move : psuedoLegal) {
        }
    }

    void genPsuedoLegalMoves(Board &board, std::vector<Board::Move> &psuedoLegal) {
        Color side = board.gameState.currentPlayer;
        Color opps = static_cast<Color>(static_cast<int>(side) ^ 1);
        bb occupied = board.bitboards[0][6] | board.bitboards[1][6];

        bb temp;

        bb friendly = board.bitboards[side][6];
        bb notfriendly = board.bitboards[opps][6];

        // generate non-pawn moves
        for (PieceType p = KNIGHT; p != N_PIECES; p = static_cast<PieceType>(static_cast<int>(p) + 1)) {
            std::cout << (int)p << std::endl;
            temp = board.bitboards[side][p];
            Board::Piece currPiece{p, side};
            while (temp) {
                Square from = bitboard::getLsbPop(temp);
                bb attackMap;

                switch(p) {
                    case KNIGHT: attackMap = knightAttackTable[from]; break;
                    case BISHOP: attackMap = genBishopMask(occupied, from); break;
                    case ROOK: attackMap = genRookMask(occupied, from); break;
                    case QUEEN:
                        attackMap = genBishopMask(occupied, from) | genRookMask(occupied, from);
                        break;
                    case KING: attackMap = kingAttackTable[from]; break;
                    case PAWN: break;
                    case N_PIECES: break;
                }

                attackMap = attackMap & (~friendly);
                bitboard::printBitboard(notfriendly);

                while (attackMap) {
                    Square to = bitboard::getLsbPop(attackMap);
                    bool isCapture = bitboard::getBit(notfriendly, to);
                    psuedoLegal.push_back({currPiece, from, to, isCapture});
                }
            }
        }

        // generate pawn moves
        temp = board.bitboards[side][PAWN];
        Board::Piece currPiece{PAWN, side};
        while (temp) {
            Square from = bitboard::getLsbPop(temp);
            bb pawnMask = bitboard::setbit(0ull, from);
            bb pawnPush, pawnAttack;
            bb promotionRange;

            switch (side) {
                case WHITE: 
                    pawnPush = whitePawnPush(pawnMask, occupied); 
                    pawnAttack = whitePawnAttacks(pawnMask, notfriendly);
                    promotionRange = horizontalMask[RANK8];
                    break;
                case BLACK: 
                    pawnPush = blackPawnAttacks(pawnMask, occupied); break;
                    pawnAttack = blackPawnAttacks(pawnMask, notfriendly);
                    promotionRange = horizontalMask[RANK1];
                    break;
                default: break;
            }
            for (bb pawnBB : {pawnPush, pawnAttack}) {
                while (pawnBB) {
                    Square to = bitboard::getLsbPop(pawnBB);
                    psuedoLegal.push_back({currPiece, from, to, false, 
                            (bool)(bitboard::setbit(0ull, to) & promotionRange)});
                }
            }
        }
    }
}
