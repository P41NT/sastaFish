#pragma once

#include "board.hpp"

#include <cstdint>

namespace zobrist {
    extern uint64_t hashTable[64][6][2];
    extern uint64_t hashCastle[16];
    extern uint64_t hashEnPassant[64];
    extern uint64_t hashSideToMove[2];

    void init();
    uint64_t hashBoard(const Board &b);
};
