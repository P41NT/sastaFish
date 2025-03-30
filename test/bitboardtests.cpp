#include "../include/bitboard.hpp"
#include <iostream>

void flipVerticalTest() {
    bb in = 0x1122448811224488;

    bitboard::printBitboard(in);
    in = bitboard::flipVertical(in);
    bitboard::printBitboard(in);
}

void flipHorizontalTest() {
    bb in = 0x1122448811224488;

    bitboard::printBitboard(in);
    in = bitboard::flipHorizontal(in);
    bitboard::printBitboard(in);
}

void testGetLSB() {
    bb in = bitboard::setbit(0ull, E5);
    in = bitboard::setbit(in, F8);
    in = bitboard::setbit(in, B2);

    bitboard::printBitboard(in);
    Square s = bitboard::getLsbPop(in);
    bitboard::printBitboard(in);

    std::cout << s << std::endl;
}

int main() {
    // flipVerticalTest();
    // flipHorizontalTest();
    testGetLSB();
}
