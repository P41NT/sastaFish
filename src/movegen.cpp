#include "../include/movegen.hpp"
#include <iostream>
#include "../include/debug.hpp"

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
        bb doublePush = bitboard::shiftNorth(bitboard::shiftNorth(pawns & 0xff00)) & (~occupied);
        return doublePush | bitboard::shiftNorth(pawns); 
    } 

    bb whitePawnPush(bb pawns, bb occupied) { 
        pawns = bitboard::shiftNorth(bitboard::shiftSouth(pawns) & (~occupied));
        bb doublePush = bitboard::shiftSouth(bitboard::shiftSouth(pawns & 0xff000000000000)) & (~occupied);
        return doublePush | bitboard::shiftSouth(pawns); 
    } 

    void preprocessknightMoves() {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                bb currbb = bitboard::setbitr(0ull, (Square)(i * 8 + j));
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
                bb currbb = bitboard::setbitr(0ull, (Square)(i * 8 + j));
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
                // diagonalMask[i + j] = bitboard::setbit(diagonalMask[i + j], (Square)(i * 8 + j));
                // antiDiagonalMask[7 + i - j] = 
                //     bitboard::setbit(antiDiagonalMask[7 + i - j], (Square)(i * 8 + j));
                bitboard::setbit(diagonalMask[i + j], (Square(i * 8 + j)));
                bitboard::setbit(antiDiagonalMask[7 + i - j], (Square(i * 8 + j)));
            }
        }
    }

    void preprocessrookMasks() {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                bitboard::setbit(horizontalMask[i], (Square)(8 * i + j));
                bitboard::setbit(verticalMask[j], (Square)(8 * i + j));
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
        s = bitboard::setbitr(s, sq);

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
        bb s = bitboard::setbitr(0ull, sq);
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

    bool isEnPassantAttacked(const std::array<std::array<bb, 7>, 2> &boards, const Square sq, const Color col) {
        bb squareBB = bitboard::setbitr(0ull, sq);
        bb occupied = boards[0][6] | boards[1][6];

        occupied &= (~squareBB);

        if (col == BLACK && whitePawnAttacks(squareBB, boards[BLACK][PAWN])) 
            return true;
        else if (col == WHITE && blackPawnAttacks(squareBB, boards[WHITE][PAWN])) 
            return true;

        return false;
    }

    Square getAttackingSquare(const std::array<std::array<bb, 7>, 2> &boards, const Square sq, const Color col) {

        if (boards[col][KING] & kingAttackTable[sq])
            return bitboard::getLsb(boards[col][KING] & kingAttackTable[sq]);
        if (boards[col][KNIGHT] & knightAttackTable[sq]) 
            return bitboard::getLsb(boards[col][KNIGHT] & knightAttackTable[sq]);


        bb squareBB = bitboard::setbitr(0ull, sq);
        bb occupied = boards[0][6] | boards[1][6];

        occupied &= (~squareBB);

        if (col == BLACK && whitePawnAttacks(squareBB, boards[BLACK][PAWN])) 
            return bitboard::getLsb(whitePawnAttacks(squareBB, boards[BLACK][PAWN]));
        else if (col == WHITE && blackPawnAttacks(squareBB, boards[WHITE][PAWN])) 
            return bitboard::getLsb(blackPawnAttacks(squareBB, boards[WHITE][PAWN]));


        bb bishopMask = genBishopMask(occupied, sq);
        if (boards[col][BISHOP] & bishopMask) 
            return bitboard::getLsb(boards[col][BISHOP] & bishopMask);

        bb rookMask = genRookMask(occupied, sq);
        if (boards[col][ROOK] & rookMask) 
            return bitboard::getLsb(boards[col][ROOK] & rookMask);

        if (boards[col][QUEEN] & (rookMask | bishopMask)) 
            return bitboard::getLsb(boards[col][QUEEN] & (rookMask | bishopMask));

        return N_SQUARES;
    }

    bool isSquareAttacked(const std::array<std::array<bb, 7>, 2> &boards, const Square sq, const Color col) {
        bb squareBB = bitboard::setbitr(0ull, sq);

        if (boards[col][KING] & kingAttackTable[sq]) return true;
        if (boards[col][KNIGHT] & knightAttackTable[sq]) return true;

        bb occupied = boards[0][6] | boards[1][6];
        occupied &= (~squareBB);

        if (col == BLACK && whitePawnAttacks(squareBB, boards[BLACK][PAWN])) return true;
        else if (col == WHITE && blackPawnAttacks(squareBB, boards[WHITE][PAWN])) return true;


        bb bishopMask = genBishopMask(occupied, sq);
        if (boards[col][BISHOP] & bishopMask) return true;

        bb rookMask = genRookMask(occupied, sq);
        if (boards[col][ROOK] & rookMask) return true;
        if (boards[col][QUEEN] & (rookMask | bishopMask)) return true;

        return false;
    }

    int countSquareAttacked(std::array<std::array<bb, 7>, 2> &boards, Square sq, Color col) {
        bb squareBB = bitboard::setbitr(0ull, sq);
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

    bb getAttackerbb(const std::array<std::array<bb, 7>, 2> &boards, Square sq, Color col) {
        bb answer = 0ull;

        bb squareBB = bitboard::setbitr(0ull, sq);
        bb occupied = boards[0][6] | boards[1][6];

        occupied &= (~squareBB);
        switch(col) {
            case BLACK: answer |= (whitePawnAttacks(squareBB, boards[BLACK][PAWN])); break;
            case WHITE: answer |= (blackPawnAttacks(squareBB, boards[WHITE][PAWN])); break;
            case N_COLORS: break;
        }

        bb bishopMask = genBishopMask(occupied, sq);
        bb rookMask = genRookMask(occupied, sq);

        answer |= (boards[col][KNIGHT] & knightAttackTable[sq]);
        answer |= (boards[col][BISHOP] & bishopMask);
        answer |= (boards[col][ROOK] & rookMask);
        answer |= (boards[col][QUEEN] & (rookMask | bishopMask));

        return answer;
    }

    std::vector<Move> genLegalMoves(Board &board) {
        std::vector<Move> psuedoLegal = genPsuedoLegalMoves(board);
        std::vector<Move> legal;

        Color side = board.currState.currentPlayer;
        Color opps = static_cast<Color>(static_cast<int>(side) ^ 1);

        bb kingBoard = board.bitboards[side][KING];
        Square kingSquare = bitboard::getLsb(kingBoard);

        bb occupied = board.bitboards[WHITE][6] | board.bitboards[BLACK][6];

        if (board.currState.isInCheck) {
            int countCheck = countSquareAttacked(board.bitboards, kingSquare, opps);
            if (countCheck == 1) {
                for (auto move :  psuedoLegal) {
                    if (move.isCastle()) continue;
                    if (board.board[move.from()].pieceType == KING) {
                        board.bitboards[side][6] ^= kingBoard;
                        bool check = isSquareAttacked(board.bitboards, move.to(), opps);
                        board.bitboards[side][6] ^= kingBoard;
                        if (!check) legal.push_back(move);
                    }
                    else {
                        bitboard::toggleBit(board.bitboards[side][6], move.to());
                        bitboard::toggleBit(board.bitboards[side][6], move.from());

                        const Piece capturePiece = board.board[move.to()];
                        if (move.isCapture()) {
                            bitboard::toggleBit(board.bitboards[opps][6], move.to());
                            bitboard::toggleBit(board.bitboards[opps][capturePiece.pieceType], move.to());
                        }

                        bool check = isSquareAttacked(board.bitboards, kingSquare, opps);

                        bitboard::toggleBit(board.bitboards[side][6], move.to());
                        bitboard::toggleBit(board.bitboards[side][6], move.from());

                        if (move.isCapture()) {
                            bitboard::toggleBit(board.bitboards[opps][6], move.to());
                            bitboard::toggleBit(board.bitboards[opps][capturePiece.pieceType], move.to());
                        }

                        if (!check) legal.push_back(move);
                    }
                }
            }
            if (countCheck >= 2) {
                for (auto move : psuedoLegal) {
                    if (move.isCastle()) continue;
                    if (board.board[move.from()].pieceType != KING) continue;
                    board.bitboards[side][6] ^= kingBoard;
                    bool check = isSquareAttacked(board.bitboards, move.to(), opps);
                    board.bitboards[side][6] ^= kingBoard;

                    if (!check) legal.push_back(move);
                }
            }
        }
        else {
            for (auto &move : psuedoLegal) {
                if (move.isCastle()) {
                    if (board.currState.currentPlayer == WHITE) {
                        if (move.move >> 12 == MoveFlag::CASTLE_KINGSIDE) {
                            if (bitboard::getBit(occupied, F1) || bitboard::getBit(occupied, G1)) 
                                continue;
                            if (isSquareAttacked(board.bitboards, F1, BLACK) ||
                                    isSquareAttacked(board.bitboards, G1, BLACK))
                                continue;
                        }
                        if (move.move >> 12 == MoveFlag::CASTLE_QUEENSIDE) {
                            if (bitboard::getBit(occupied, D1) || 
                                    bitboard::getBit(occupied, C1) ||
                                    bitboard::getBit(occupied, B1)) 
                                continue;
                            if (isSquareAttacked(board.bitboards, D1, BLACK) || 
                                    isSquareAttacked(board.bitboards, C1, BLACK))
                                continue;
                        }
                    }
                    else {
                        if (move.move >> 12 == MoveFlag::CASTLE_KINGSIDE) {
                            if (bitboard::getBit(occupied, F8) || 
                                    bitboard::getBit(occupied, G8))
                                continue;
                            if (isSquareAttacked(board.bitboards, F8, WHITE) ||
                                    isSquareAttacked(board.bitboards, G8, WHITE))
                                continue;
                        }
                        if (move.move >> 12 == MoveFlag::CASTLE_QUEENSIDE) {
                            if (bitboard::getBit(occupied, D8) || 
                                    bitboard::getBit(occupied, C8) ||
                                    bitboard::getBit(occupied, B8)) 
                                continue;
                            if (isSquareAttacked(board.bitboards, D8, WHITE) || 
                                    isSquareAttacked(board.bitboards, C8, WHITE))
                                continue;
                        }
                    }
                    legal.push_back(move);
                }
                else if (move.isEnPassant()) {
                    bitboard::toggleBit(board.bitboards[side][6], move.from());
                    bitboard::toggleBit(board.bitboards[side][6], board.currState.enPassantSquare);

                    Square attackSquare = board.currState.enPassantSquare;
                    if (board.currState.currentPlayer == WHITE) 
                        attackSquare = static_cast<Square>(static_cast<int>(attackSquare) + 8);
                    else 
                        attackSquare = static_cast<Square>(static_cast<int>(attackSquare) - 8);

                    bitboard::toggleBit(board.bitboards[opps][6], attackSquare);

                    bool check = isSquareAttacked(board.bitboards, kingSquare, opps);

                    bitboard::toggleBit(board.bitboards[side][6], move.from());
                    bitboard::toggleBit(board.bitboards[side][6], board.currState.enPassantSquare);
                    bitboard::toggleBit(board.bitboards[opps][6], attackSquare);

                    if (!check) legal.push_back(move);
                }
                else {
                    if (board.board[move.from()].pieceType == KING) {
                        if (!isSquareAttacked(board.bitboards, move.to(), opps))
                            legal.push_back(move);
                    }
                    else {
                        bitboard::toggleBit(board.bitboards[side][6], move.from());
                        bitboard::toggleBit(board.bitboards[side][6], move.to());

                        Piece capturePiece = board.board[move.to()];
                        
                        if (move.isCapture()) {
                            bitboard::toggleBit(board.bitboards[opps][6], move.to());
                            bitboard::toggleBit(board.bitboards[opps][capturePiece.pieceType], move.to());
                        }

                        bool check = isSquareAttacked(board.bitboards, kingSquare, opps);

                        bitboard::toggleBit(board.bitboards[side][6], move.from());
                        bitboard::toggleBit(board.bitboards[side][6], move.to());

                        if (move.isCapture()) {
                            bitboard::toggleBit(board.bitboards[opps][6], move.to());
                            bitboard::toggleBit(board.bitboards[opps][capturePiece.pieceType], move.to());
                        }

                        if (!check) legal.push_back(move);
                    }
                }
            }
        }
        return legal;
    }

    std::vector<Move> genPsuedoLegalMoves(Board &board) {
        std::vector<Move> psuedoLegal;

        Color side = board.currState.currentPlayer;
        Color opps = static_cast<Color>(static_cast<int>(side) ^ 1);
        bb occupied = board.bitboards[0][6] | board.bitboards[1][6];

        bb temp;

        bb friendly = board.bitboards[side][6];
        bb notfriendly = board.bitboards[opps][6];

        // generate non-pawn moves
        for (PieceType p = KNIGHT; p != N_PIECES; p = static_cast<PieceType>(static_cast<int>(p) + 1)) {
            temp = board.bitboards[side][p];
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
                    if (isCapture) psuedoLegal.push_back(Move(from, to, CAPTURE));
                    else psuedoLegal.push_back(Move(from, to, QUIET));
                }
            }
        }

        Piece currPiece = {KING, side};
        // generate castling moves
        switch (side) {
            case WHITE:
                if (CASTLE_KING_WHITE & board.currState.castlingState) {
                    psuedoLegal.push_back(Move(E1, G1, MoveFlag::CASTLE_KINGSIDE));
                }
                if (CASTLE_QUEEN_WHITE & board.currState.castlingState) {
                    psuedoLegal.push_back(Move(E1, C1, MoveFlag::CASTLE_QUEENSIDE));
                }
                break;
            case BLACK:
                if (CASTLE_KING_BLACK & board.currState.castlingState) {
                    psuedoLegal.push_back(Move(E8, G8, MoveFlag::CASTLE_KINGSIDE));
                }
                if (CASTLE_QUEEN_BLACK & board.currState.castlingState) {
                    psuedoLegal.push_back(Move(E8, C8, MoveFlag::CASTLE_QUEENSIDE));
                }
                break;
            default: break;
        }

        // generate pawn moves
        temp = board.bitboards[side][PAWN];
        currPiece = {PAWN, side};
        while (temp) {
            Square from = bitboard::getLsbPop(temp);
            bb pawnMask = bitboard::setbitr(0ull, from);
            bb pawnPush, pawnAttack;
            bb promotionRange = 0ull;

            bb enPassantTemp = 0ull;
            if (board.currState.enPassantSquare != N_SQUARES) {
                enPassantTemp = bitboard::setbitr(0ull, board.currState.enPassantSquare);
            }
            bb enPassant;

            switch (side) {
                case WHITE: 
                    pawnPush = whitePawnPush(pawnMask, occupied); 
                    pawnAttack = whitePawnAttacks(pawnMask, notfriendly);
                    enPassant = whitePawnAttacks(pawnMask, enPassantTemp); 
                    promotionRange = horizontalMask[RANK8];
                    break;
                case BLACK: 
                    pawnPush = blackPawnPush(pawnMask, occupied);
                    pawnAttack = blackPawnAttacks(pawnMask, notfriendly);
                    enPassant = blackPawnAttacks(pawnMask, enPassantTemp);
                    promotionRange = horizontalMask[RANK1];
                    break;
                default: break;
            }
            while (pawnPush) {
                Square to = bitboard::getLsbPop(pawnPush);
                if (bitboard::setbitr(0ull, to) & promotionRange) {
                    psuedoLegal.push_back(Move(from, to, PROMOTE_B));
                    psuedoLegal.push_back(Move(from, to, PROMOTE_N));
                    psuedoLegal.push_back(Move(from, to, PROMOTE_R));
                    psuedoLegal.push_back(Move(from, to, PROMOTE_Q));
                }
                else {
                    if (abs(to - from) == 16) psuedoLegal.push_back(Move(from, to, DOUBLE_PUSH));
                    else psuedoLegal.push_back(Move(from, to, QUIET));
                }
            }
            while (pawnAttack) {
                Square to = bitboard::getLsbPop(pawnAttack);
                if (bitboard::setbitr(0ull, to) & promotionRange) {
                    psuedoLegal.push_back(Move(from, to, CAPTURE | PROMOTE_B));
                    psuedoLegal.push_back(Move(from, to, CAPTURE | PROMOTE_N));
                    psuedoLegal.push_back(Move(from, to, CAPTURE | PROMOTE_R));
                    psuedoLegal.push_back(Move(from, to, CAPTURE | PROMOTE_Q));
                }
                else {
                    psuedoLegal.push_back(Move(from, to, CAPTURE));
                }
            }
            while (enPassant) {
                Square to = bitboard::getLsbPop(enPassant);
                psuedoLegal.push_back(Move(from, to, EN_PASSANT));
            }
        }
        return psuedoLegal;
    }
}
