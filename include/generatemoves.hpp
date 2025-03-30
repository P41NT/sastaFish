#pragma once

#include "bitboard.hpp"
#include "board.hpp"

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

    bb genRookAttacks(bb friendly, bb occupied, Square sq);
    bb genBishopAttacks(bb friendly, bb occupied, Square sq);
    bb genQueenAttacks(bb friendly, bb occupied, Square sq);

    bb getKnightAttacks(bb friendly, Square sq);
    bb getKingAttacks(bb friendly, Square sq);

    bool isSquareAttacked(std::array<std::array<bb, 7>, 2> &boards, Square sq, Color col);

    // void genPsuedoLegalMoves(Board &board, std::vector<Board::Move> &psuedolegal);
    void genLegalMoves(Board &board, std::vector<Board::Move> &legal);
}
