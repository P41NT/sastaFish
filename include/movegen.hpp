#pragma once

#include "bitboard.hpp"
#include "board.hpp"
#include <array>
#include <vector>

struct MoveList {
    std::array<Move, 256> moves;

    size_t count = 0;
    inline void push_back(const Move& m) { moves[count++] = m; }

    inline size_t size() { return count;  }
    inline bool empty() { return count == 0;  }

    inline Move* begin() { return &moves[0]; }
    inline Move* end() { return &moves[count]; }

    inline const Move* begin() const { return &moves[0]; }
    inline const Move* end() const { return &moves[count]; }

    inline const Move& operator[](size_t index) const {
        return moves[index];
    }

    template<typename... Args>
    inline void emplace_back(Args&&... args) {
        moves[count++] = Move(std::forward<Args>(args)...);
    }
};

namespace moveGen {
    extern bb knightAttackTable[64];
    extern bb kingAttackTable[64];
    extern bb diagonalMask[15];
    extern bb antiDiagonalMask[15];
    extern bb horizontalMask[8];
    extern bb verticalMask[8];

    bb whitePawnAttacks(bb pawns, bb opponents);
    bb blackPawnAttacks(bb pawns, bb opponents);
    bb whitePawnPush(bb pawns, bb occupied);
    bb blackPawnPush(bb pawns, bb occupied); 

    void init();
    void preprocessknightMoves();
    void preprocesskingMoves();
    void preprocessbishopMasks();
    void preprocessrookMasks();

    bb genBishopMask(bb occupied, Square sq);
    bb genRookMask(bb occupied, Square sq);

    Square getAttackingSquare(const std::array<std::array<bb, 7>, 2> &boards, const Square sq, const Color col);
    bool isSquareAttacked(const std::array<std::array<bb, 7>, 2> &boards, const Square sq, const Color col);
    bool isSquareAttackedWithOccupied(const std::array<std::array<bb, 7>, 2> &boards, const Square sq, const Color col, bb occupied);
    int countSquareAttacked(const std::array<std::array<bb, 7>, 2> &boards, Square sq, Color col);
    bb getAttackerbb(const std::array<std::array<bb, 7>, 2> &boards, Square sq, Color col);

    bool isEnPassantAttacked(const std::array<std::array<bb, 7>, 2> &boards, const Square sq, const Color col);

    void genPsuedoLegalMoves(Board &board, MoveList& movelist);
    void genLegalMoves(Board &board, MoveList& movelist);
}
