#include "../include/zobrist.hpp"

#include <random>
#include <chrono>

namespace zobrist {
    uint64_t hashTable[64][6][2];
    uint64_t hashCastle[16];
    uint64_t hashEnPassant[64];
    uint64_t hashSideToMove[2];

    void init() {
        std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        for (int boardIndex = 0; boardIndex < 64; boardIndex++) {
            hashEnPassant[boardIndex] = rng();
            for (int pieceIndex = 0; pieceIndex < 6; pieceIndex++) {
                for (int side = 0; side < 2; side++) {
                    hashTable[boardIndex][pieceIndex][side] = rng();
                }
            }
        }
        for (int castleInd = 0; castleInd < 16; castleInd++) hashCastle[castleInd] = rng();
        for (int side = 0; side < 2; side++) hashSideToMove[side] = rng();
    }

    uint64_t hashBoard(const Board &b) {
        uint64_t hash = 0;
        for (int i = 0; i < 64; i++) {
            if (b.board[i].pieceType != PieceType::N_PIECES) {
                int p = static_cast<int>(b.board[i].pieceType);
                int c = static_cast<int>(b.board[i].color);
                hash ^= hashTable[i][p][c];
            }
        }
        hash ^= hashCastle[b.currState.castlingState];
        hash ^= hashEnPassant[b.currState.enPassantSquare];
        return hash;
    }
}
