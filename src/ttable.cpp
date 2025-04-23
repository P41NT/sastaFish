#include "../include/ttable.hpp"

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
