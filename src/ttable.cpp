#include "../include/ttable.hpp"
#include <cstdint>
#include <cstdlib>

void TTable::setEntry(uint64_t hash, int depth, int score, TTableFlag flag, Move bestMove) {
    TTableEntry &entry = transpositionTable[hash % TTableSize];
    if (depth < entry.depth) return;

    entry.hash = hash;
    entry.depth = depth;
    entry.score = score;
    entry.flag = flag;
    entry.bestMove = bestMove;
}
TTableEntry* TTable::getEntry(uint64_t hash) {
    TTableEntry &result = this->transpositionTable[hash % TTableSize];
    if (result.hash == hash) return &result;
    return nullptr;
}

int RepetitionTable::getEntry(uint64_t hash) {
    return repetitionTable[hash % TTableSize];
}

void RepetitionTable::increment(uint64_t hash) {
    hashStack.push(hash);
    repetitionTable[hash % TTableSize]++;
}

void RepetitionTable::decrement(uint64_t hash) {
    repetitionTable[hash % TTableSize]--;
}
