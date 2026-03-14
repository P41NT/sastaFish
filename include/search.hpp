#pragma once

#include "board.hpp"
#include "openingbook.hpp"
#include "ttable.hpp"

#include <atomic>
#include <chrono>

namespace search {

    using timestamp = std::chrono::time_point<std::chrono::high_resolution_clock>;

    int alphaBetaSearch(Board& b, TTable& tt, RepetitionTable& rt, int alpha, int beta, int depth, int& nodes, bool& stopSearch, timestamp deadline);
    Move bestMove(Board &b, TTable &tt, RepetitionTable &rt, book::Book &bk, int maxDepth, int maxTime, int &nodes, int &depth, int &score);
    Move iterativeDeepening(Board &b, TTable &tt, RepetitionTable &rt, int maxDepth, int maxTime, int &nodes, int &depth, int &score);
    int quiesce(Board &b, TTable &tt, int alpha, int beta, int depth, bool &stopSearch, int &nodes, timestamp deadline);
}
