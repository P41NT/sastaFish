#include "../include/generatemoves.hpp"

void knightMoveTest() {
    moveGen::preprocessknightMoves();
    bitboard::printBitboard(moveGen::knightAttackTable[4]);
    bitboard::printBitboard(moveGen::knightAttackTable[30]);
}

void rookMoveTest() {
    bb blockers = 0ull;
    bb rook = 0ull;

    rook = bitboard::setbit(rook, E5);

    blockers = bitboard::setbit(blockers, E2);
    blockers = bitboard::setbit(blockers, E8);
    blockers = bitboard::setbit(blockers, A5);
    blockers = bitboard::setbit(blockers, G8);
    blockers = bitboard::setbit(blockers, G5);

    bb attacks = moveGen::genRookAttacks(blockers, blockers, Square::E5);

    bitboard::printBitboard(blockers);
    bitboard::printBitboard(rook);
    bitboard::printBitboard(attacks);
}

void bishopMoveTest() {
    moveGen::preprocessbishopMasks();

    bb blockers = 0ull;
    bb bishop = 0ull;

    bishop = bitboard::setbit(bishop, C3);

    blockers = bitboard::setbit(blockers, A1);
    blockers = bitboard::setbit(blockers, F6);
    blockers = bitboard::setbit(blockers, D2);

    bb attacks = moveGen::genBishopAttacks(blockers, blockers, Square::C3);

    bitboard::printBitboard(bishop);
    bitboard::printBitboard(blockers);
    bitboard::printBitboard(attacks);
}

int main() {
    bishopMoveTest();
}
