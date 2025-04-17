#pragma once

#include "board.hpp"

namespace eval {
    static constexpr int pieceValues[6] = { 1, 3, 3, 6, 9, 10000 };
    static const int materialWeight = 2;
    static const int mobilityWeight = 3;

    int evaluateBoard(Board &b);
    int materialScore(Board &b);
    int mobilityScore(Board &b);

    int quiesce(Board &b, int alpha, int beta, int depth);
}

