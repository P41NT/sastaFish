#pragma once

#include "board.hpp"
#include "openingbook.hpp"
#include "ttable.hpp"

#include <atomic>

namespace search {
    int alphaBetaSearch(Board &b, TTable &tt, RepetitionTable &rt, int alpha, int beta, int depth, int &nodes, std::atomic<bool> &stopSearch, bool debug);
    Move bestMove(Board &b, TTable &tt, RepetitionTable &rt, book::Book &bk, int maxDepth, int maxTime, int &nodes, int &depth, int &score);
    Move iterativeDeepening(Board &b, TTable &tt, RepetitionTable &rt, int maxDepth, int maxTime, int &nodes, int &depth, int &score);
    int quiesce(Board &b, int alpha, int beta, int depth, std::atomic<bool> &stopSearch);
}
