#pragma once

#include "board.hpp"

namespace debug {
    inline std::string printPiece(const Piece p) {
        if (p.pieceType == N_PIECES) return ".";
        return pieceCharacters[p.color][p.pieceType]; 
    }
    void printMove(const Board &b, const Move &mv);
    void printBoard(const Board &b);
    void printBitboard(const bb &in);
}
