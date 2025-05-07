#pragma once

#include "utils.hpp"
#include <cctype>

/*
 * 1000 -> captures 
 * 1100 -> promote capture knight
 * 1101 -> promote capture bishop
 * 1110 -> promote capture rook
 * 1111 -> promote capture queen
 * 1001 -> en passant
 *
 * 0100 -> promote to knight 
 * 0101 -> promote to bishop 
 * 0110 -> promote to rook
 * 0111 -> promote to queen
 *
 * 0010 -> castle kingside
 * 0011 -> castle queenside
 */

enum MoveFlag : uint8_t {
    EN_PASSANT = 0b1001,
    CAPTURE = 0b1000,
    PROMOTE = 0b0100,

    PROMOTE_B = 0b0100,
    PROMOTE_N = 0b0101,
    PROMOTE_R = 0b0110,
    PROMOTE_Q = 0b0111,

    CASTLE = 0b0010,
    QUIET = 0b0000,

    CASTLE_KINGSIDE = 0b0010,
    CASTLE_QUEENSIDE = 0b0001,
    DOUBLE_PUSH = 0b0011
};

class Move {
public:
    uint16_t move;
    inline Move() : move(0) {}

    inline Move(Square from, Square to) : move(static_cast<uint8_t>(from) << 6 | static_cast<uint8_t>(to)) {}
    inline Move(Square from, Square to, uint8_t flags) : 
        move((flags << 12) | (static_cast<uint8_t>(from) << 6) | (static_cast<uint8_t>(to))) { }

    inline bool isCapture() const { return (move >> 12 & 0b1000); }
    inline bool isPromotion() const { return (move >> 12 & 0b0100); }
    inline bool isCastle() const { return ((move >> 12) == CASTLE_KINGSIDE 
            || (move >> 12) == CASTLE_QUEENSIDE); }
    inline bool isEnPassant() const { return ((move >> 12) == MoveFlag::EN_PASSANT); }
    inline bool isDoublePush() const { return ((move >> 12) == MoveFlag::DOUBLE_PUSH); }

    inline std::string getUciString() const { 
        return squareWord[from()] + squareWord[to()] + (isPromotion() ? pieceLetters[from() > A2][promotionPiece()] : "");
    }

    inline PieceType promotionPiece() const {
        switch ((move >> 12) & (~CAPTURE)) {
            case PROMOTE_B: return BISHOP; break;
            case PROMOTE_N: return KNIGHT; break;
            case PROMOTE_R: return ROOK; break;
            case PROMOTE_Q: return QUEEN; break;
            default: return PAWN;
        }
    }

    inline Square from() const { return static_cast<Square>((move >> 6) & 0b111111); }
    inline Square to() const { return static_cast<Square>(move & 0b111111); }

    bool operator==(Move &other) const { return this->move == other.move; }
    bool operator!=(Move &other) const { return this->move == other.move; }
};

