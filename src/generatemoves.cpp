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

                kingAttackTable[i * 8 + j] = attackbb;
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

        occupied &= (~squareBB);
        if (col == BLACK && whitePawnAttacks(squareBB, boards[BLACK][PAWN])) 
            return bitboard::getLsb(whitePawnAttacks(squareBB, boards[BLACK][PAWN]));
        else if (col == WHITE && blackPawnAttacks(squareBB, boards[WHITE][PAWN])) 
            return bitboard::getLsb(blackPawnAttacks(squareBB, boards[WHITE][PAWN]));

        bb bishopMask = genBishopMask(occupied, sq);
        bb rookMask = genRookMask(occupied, sq);

        if (boards[col][KING] & kingAttackTable[sq])
            return bitboard::getLsb(boards[col][KING] & kingAttackTable[sq]);
        if (boards[col][KNIGHT] & knightAttackTable[sq]) 
            return bitboard::getLsb(boards[col][KNIGHT] & knightAttackTable[sq]);
        if (boards[col][BISHOP] & bishopMask) 
            return bitboard::getLsb(boards[col][BISHOP] & bishopMask);
        if (boards[col][ROOK] & rookMask) 
            return bitboard::getLsb(boards[col][ROOK] & rookMask);
        if (boards[col][QUEEN] & (rookMask | bishopMask)) 
            return bitboard::getLsb(boards[col][QUEEN] & (rookMask | bishopMask));

        return N_SQUARES;
    }

    int countSquareAttacked(std::array<std::array<bb, 7>, 2> &boards, Square sq, Color col) {
        bb squareBB = bitboard::setbit(0ull, sq);
        bb occupied = boards[0][6] | boards[1][6];

        int answer = 0;

        occupied &= (~squareBB);
        switch(col) {
            case BLACK: answer += bitboard::numBits(whitePawnAttacks(squareBB, boards[BLACK][PAWN])); break;
            case WHITE: answer += bitboard::numBits(blackPawnAttacks(squareBB, boards[WHITE][PAWN])); break;
            case N_COLORS: break;
        }

        bb bishopMask = genBishopMask(occupied, sq);
        bb rookMask = genRookMask(occupied, sq);

        answer += bitboard::numBits(boards[col][KNIGHT] & knightAttackTable[sq]);
        answer += bitboard::numBits(boards[col][BISHOP] & bishopMask);
        answer += bitboard::numBits(boards[col][ROOK] & rookMask);
        answer += bitboard::numBits(boards[col][QUEEN] & (rookMask | bishopMask));

        return answer;
    }

    void genLegalMoves(Board &board, std::vector<Move> &legal) {
        std::vector<Move> psuedoLegal;
        genPsuedoLegalMoves(board, psuedoLegal);

        Color side = board.gameState.currentPlayer;
        Color opps = static_cast<Color>(static_cast<int>(side) ^ 1);

        bb kingBoard = board.bitboards[side][KING];
        Square kingSquare = bitboard::getLsb(kingBoard);

        bb occupied = board.bitboards[WHITE][6] | board.bitboards[BLACK][6];

        if (board.gameState.isInCheck) {
            int countCheck = countSquareAttacked(board.bitboards, kingSquare, opps);
            std::cout << "Count check " << countCheck << std::endl;
            if (countCheck == 1) {
                Square attacker = isSquareAttacked(board.bitboards, kingSquare, opps);
                for (auto move :  psuedoLegal) {
                    if (board.board[move.from()].pieceType == KING) {
                        board.bitboards[side][6] ^= kingBoard;
                        Square check = isSquareAttacked(board.bitboards, move.to(), opps);
                        board.bitboards[side][6] ^= kingBoard;
                        if (check == N_SQUARES) legal.push_back(move);
                    }
                    else if (move.to() == attacker) legal.push_back(move);
                    else {
                        board.bitboards[side][6] ^= bitboard::setbit(0ull, move.from());
                        board.bitboards[side][6] ^= bitboard::setbit(0ull, move.to());
                        Square check = isSquareAttacked(board.bitboards, kingSquare, opps);
                        board.bitboards[side][6] ^= bitboard::setbit(0ull, move.from());
                        board.bitboards[side][6] ^= bitboard::setbit(0ull, move.to());

                        if (check == N_SQUARES) legal.push_back(move);
                    }
                }
            }
            if (countCheck >= 2) {
                for (auto move : psuedoLegal) {
                    if (board.board[move.from()].pieceType != KING) continue;
                    board.bitboards[side][6] ^= kingBoard;
                    Square check = isSquareAttacked(board.bitboards, move.to(), opps);
                    board.bitboards[side][6] ^= kingBoard;
                    if (check == N_SQUARES) legal.push_back(move);
                }
            }
        }
        else {
            for (auto move : psuedoLegal) {
                board.bitboards[side][6] ^= bitboard::setbit(0ull, move.from());
                board.bitboards[side][6] ^= bitboard::setbit(0ull, move.to());
                Square check = isSquareAttacked(board.bitboards, kingSquare, opps);
                board.bitboards[side][6] ^= bitboard::setbit(0ull, move.from());
                board.bitboards[side][6] ^= bitboard::setbit(0ull, move.to());

                if (check == N_SQUARES) legal.push_back(move);
            }
        }
    }

    void genPsuedoLegalMoves(Board &board, std::vector<Move> &psuedoLegal) {
        Color side = board.gameState.currentPlayer;
        Color opps = static_cast<Color>(static_cast<int>(side) ^ 1);
        bb occupied = board.bitboards[0][6] | board.bitboards[1][6];

        bb temp;

        bb friendly = board.bitboards[side][6];
        bb notfriendly = board.bitboards[opps][6];

        // generate non-pawn moves
        for (PieceType p = KNIGHT; p != N_PIECES; p = static_cast<PieceType>(static_cast<int>(p) + 1)) {
            temp = board.bitboards[side][p];
            Piece currPiece{p, side};
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
                    case KING: 
                        attackMap = kingAttackTable[from]; break;
                    default: break;
                }

                attackMap = attackMap & (~friendly);

                while (attackMap) {
                    Square to = bitboard::getLsbPop(attackMap);
                    bool isCapture = bitboard::getBit(notfriendly, to);
                    // psuedoLegal.push_back({currPiece, from, to, isCapture, false, false});
                }
            }
        }

        Piece currPiece = {KING, side};
        // generate castling moves
        switch (side) {
            case WHITE:
                if (CASTLE_KING_WHITE & board.gameState.castlingState) {
                    // psuedoLegal.push_back({currPiece, E1, G1, false, false, false,
                    //         N_PIECES, CASTLE_KING_WHITE});
                }
                if (CASTLE_QUEEN_WHITE & board.gameState.castlingState) {
                    // psuedoLegal.push_back({currPiece, E1, C1, false, false, false,
                    //         N_PIECES, CASTLE_QUEEN_WHITE});
                }
                break;
            case BLACK:
                if (CASTLE_KING_BLACK & board.gameState.castlingState) {
                    // psuedoLegal.push_back({currPiece, E8, G8, false, false, false,
                    //         N_PIECES, CASTLE_KING_BLACK});
                }
                if (CASTLE_QUEEN_BLACK & board.gameState.castlingState) {
                    // psuedoLegal.push_back({currPiece, E8, C8, false, false, false,
                    //         N_PIECES, CASTLE_QUEEN_BLACK});
                }
                break;
            default: break;
        }

        // generate pawn moves
        temp = board.bitboards[side][PAWN];
        currPiece = {PAWN, side};
        while (temp) {
            Square from = bitboard::getLsbPop(temp);
            bb pawnMask = bitboard::setbit(0ull, from);
            bb pawnPush, pawnAttack;
            bb promotionRange;

            bb enPassant;

            switch (side) {
                case WHITE: 
                    pawnPush = whitePawnPush(pawnMask, occupied); 
                    pawnAttack = whitePawnAttacks(pawnMask, notfriendly);
                    enPassant = whitePawnAttacks(pawnMask, 
                            bitboard::setbit(0ull, board.gameState.enPassantSquare));
                    promotionRange = horizontalMask[RANK8];
                    break;
                case BLACK: 
                    pawnPush = blackPawnAttacks(pawnMask, occupied); break;
                    pawnAttack = blackPawnAttacks(pawnMask, notfriendly);
                    enPassant = blackPawnAttacks(pawnMask, 
                            bitboard::setbit(0ull, board.gameState.enPassantSquare));
                    promotionRange = horizontalMask[RANK1];
                    break;
                default: break;
            }

            while (pawnPush) {
                Square to = bitboard::getLsbPop(pawnPush);
                // psuedoLegal.push_back({currPiece, from, to, false,
                //         (bool)(bitboard::setbit(0ull, to) & promotionRange), false});
            }
            while (pawnAttack) {
                Square to = bitboard::getLsbPop(pawnAttack);
                // psuedoLegal.push_back({currPiece, from, to, true,
                //         (bool)(bitboard::setbit(0ull, to) & promotionRange), false});
            }
            while (enPassant) {
                Square to = bitboard::getLsbPop(enPassant);
                // psuedoLegal.push_back({currPiece, from, to, true, false, true});
            }
        }
    }
}
