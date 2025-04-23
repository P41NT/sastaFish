#pragma once

#include "board.hpp"
#include "ttable.hpp"

namespace search {
    int alphaBetaSearch(Board &b, TTable &tt, int alpha, int beta, int depth, int &nodes);
    Move bestMove(Board &b, TTable &tt, int &nodes, int &depth, int &score);
    Move iterativeDeepening(Board &b, TTable &tt, int maxDepth, int maxTime, int &nodes, int &depth, int &score);
}
