#pragma once

#include "board.hpp"

namespace search {
    int alphaBetaSearch(Board &b, int alpha, int beta, int depth);
    Move bestMove(Board &b);
}
