#pragma once

#include "utils.hpp"
#include <cstdint>

using bb = uint64_t;

namespace bitboard {
    static const int bitScanLookup[68] = {
      64,  0,  1, 39,  2, 15, 40, 23,
       3, 12, 16, 59, 41, 19, 24, 54,
       4, -1, 13, 10, 17, 62, 60, 28,
      42, 30, 20, 51, 25, 44, 55, 47,
       5, 32, -1, 38, 14, 22, 11, 58,
      18, 53, 63,  9, 61, 27, 29, 50,
      43, 46, 31, 37, 21, 57, 52,  8,
      26, 49, 45, 36, 56,  7, 48, 35,
       6, 34, 33, -1
    };

    inline bb shiftSouth(const bb in) { return in >> 8; }
    inline bb shiftNorth(const bb in) { return in << 8; }
    inline bb shiftWest(const bb in) { return (in << 1) & 0xFEFEFEFEFEFEFEFE; }
    inline bb shiftEast(const bb in) { return (in >> 1) & 0x7F7F7F7F7F7F7F7F; }

    inline bb shiftSouthWest(const bb in) { return (in >> 7) & 0xFEFEFEFEFEFEFEFE; }
    inline bb shiftSouthEast(const bb in) { return (in >> 9) & 0x7F7F7F7F7F7F7F7F; }
    inline bb shiftNorthWest(const bb in) { return (in << 9) & 0xFEFEFEFEFEFEFEFE; }
    inline bb shiftNorthEast(const bb in) { return (in << 7) & 0x7F7F7F7F7F7F7F7F; }

    inline bool getBit(const bb in, const Square sq) { return static_cast<bool>((in >> sq) & 1ull); }

    inline bb setbitr(const bb in, const Square sq) { return (in | (1ull << sq)); }
    inline bb toggleBitr(const bb in, const Square sq) { return (in ^ (1ull << sq)); }

    inline void setbit(bb &in, const Square sq) { in = (in | (1ull << sq)); }
    inline void toggleBit(bb &in, const Square sq) { in = (in ^ (1ull << sq)); }

    bb flipVertical(const bb &in);
    bb flipHorizontal(const bb &in);

    Square getLsb(const bb &in);
    Square getLsbPop(bb &in);

    inline int numBits(bb in) { return __builtin_popcountll(in); }
};
