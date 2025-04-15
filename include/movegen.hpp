#pragma once

#include "bitboard.hpp"
#include "board.hpp"
#include <vector>

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
    int countSquareAttacked(const std::array<std::array<bb, 7>, 2> &boards, Square sq, Color col);

    std::vector<Move> genPsuedoLegalMoves(Board &board);
    std::vector<Move> genLegalMoves(Board &board);
}
