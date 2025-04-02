#include "utils.hpp"

/*
 * 1000 -> captures 
 * 1100 -> promote capture knight
 * 1101 -> promote capture bishop
 * 1110 -> promote capture rook
 * 1111 -> promote capture queen
 * 1001 -> en passant
 *
 * 0100 -> promotions
 * 0101 -> promote to knight
 * 0110 -> promote to rook
 * 0111 -> promote to queen
 *
 * 0010 -> castle kingside
 * 0011 -> castle queenside
 */

enum MoveFlag : int {
    EN_PASSANT = 0b1001,
    CAPTURE = 0b1000,
    PROMOTE = 0b0100,

    CASTLE = 0b0010,
    QUIET = 0b0000,

    CASTLE_KINGSIDE = 0b0010,
    CASTLE_QUEENSIDE = 0b0001
};

class Move {
private:
    uint16_t move;
public:
    inline Move() : move(0) {}

    inline Move(Square from, Square to) {
        move = ((uint8_t)from << 6) | (uint8_t)to;
    }

    inline Move(Square from, Square to, uint8_t flags) {
        move = (flags << 12) | ((uint8_t)from << 6) | ((uint8_t)to);
    }

    inline bool isCapture() const { return (move >> 12 & 0b1000); }
    inline bool isPromotion() const { return (move >> 12 & 0b0100); }

    inline Square from() const { return (Square)((move >> 6) & 0b111111); }
    inline Square to() const { return (Square)(move & 0b111111); }

    void operator=(Move other) { move = other.move; }
    bool operator==(Move &other) const { return this->move == other.move; }
    bool operator!=(Move &other) const { return this->move == other.move; }
};

