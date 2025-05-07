#pragma once

#include <array>
#include <cstdint>

#include "move.hpp"

enum TTableFlag {
    EXACT = 0,
    LOWERBOUND = 1,
    UPPERBOUND = 2
};
struct TTableEntry {
    uint64_t hash;
    int depth;
    int score;
    Move bestMove;
    TTableFlag flag;
};

static const int TTableSize = 1 << 20;
class TTable {
    std::array<TTableEntry, TTableSize> transpositionTable;

public:
    TTableEntry* getEntry(uint64_t hash);
    void setEntry(uint64_t hash, int depth, int score, TTableFlag flag, Move bestMove);
};

class RepetitionTable {
    std::array<int, TTableSize> repetitionTable;

public:
    int getEntry(uint64_t hash);
    void increment(uint64_t hash);
    void decrement(uint64_t hash);
};
