#pragma once

#include "board.hpp"

#include <cstdint>
#include <memory>

namespace zobrist {
    extern uint64_t hashTable[64][6][2];
    extern uint64_t hashCastle[16];
    extern uint64_t hashEnPassant[64];
    extern uint64_t hashSideToMove[2];

    void init();
    uint64_t hashBoard(std::shared_ptr<Board> b);
};
