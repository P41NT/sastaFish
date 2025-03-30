#include <iostream>

#include "../include/bitboard.hpp"

namespace bitboard {
    bb flipVertical(bb in) {
        bb mask1 = 0xFF00FF00FF00FF00;
        bb mask2 = 0xFFFF0000FFFF0000;

        bb x = in;
        x = ((x & mask1) >> 8) | ((x << 8) & mask1);
        x = ((x & mask2) >> 16) | ((x << 16) & mask2);
        x = (x >> 32) | (x << 32);

        return x;
    }

    bb flipHorizontal(bb in) {
        bb mask1 = 0x5555555555555555;
        bb mask2 = 0x3333333333333333;
        bb mask3 = 0x0F0F0F0F0F0F0F0F;

        bb x = in;
        x = ((x & mask1) << 1) | ((x >> 1) & mask1);
        x = ((x & mask2) << 2) | ((x >> 2) & mask2);
        x = ((x & mask3) << 4) | ((x >> 4) & mask3);

        return x;
    }

    void printBitboard(bb in) {
        for (int rank = 0; rank < 8; rank++) {
            for (int file = 0; file < 8; file++) {
                int square = rank * 8 + file;
                if (!file) std::cout << 8 - rank << "\t";
                std::cout << getBit(in, (Square)(rank * 8 + file)) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "\n\ta b c d e f g h\n" << std::endl;
    }

    Square getLsb(bb in) {
        bb lsb_bb = in & (-in);
        return (Square)bitScanLookup[in % 67];
    }

    Square getLsbPop(bb &in) {
        bb lsb_bb = in & (-in);
        Square answer = (Square)bitScanLookup[lsb_bb % 67];
        in ^= lsb_bb;
        return answer;
    }
}
