#include "../include/zobrist.hpp"
#include "../include/movegen.hpp"

namespace zobrist {
    uint64_t getPieceHash(Piece p, Square square) {
        int pieceNumber = (p.pieceType * 2) + (p.color == BLACK ? 0 : 1);
        int squareNumber = polyglotMap[square];
        int offset = pieceNumber * 64 + squareNumber;
        return Random64[offset];
    }

    uint64_t getCastleHash(uint8_t c) {
        static const int castleOffset = 768;
        uint64_t hash = 0;
        if (c & CASTLE_KING_WHITE)  hash ^= Random64[castleOffset + 0];
        if (c & CASTLE_QUEEN_WHITE) hash ^= Random64[castleOffset + 1];
        if (c & CASTLE_KING_BLACK)  hash ^= Random64[castleOffset + 2]; 
        if (c & CASTLE_QUEEN_BLACK) hash ^= Random64[castleOffset + 3];
        return hash;
    }

    uint64_t getEnPassantHash(Square square) {
        static const int enPassantOffset = 772;
        if (square == N_SQUARES) return 0;
        return Random64[enPassantOffset + (square % 8)];
    }

    uint64_t getTurnHash(Color curr) {
        static const int turnOffset = 780;
        if (curr == BLACK) return 0;
        return Random64[turnOffset];
    }

    uint64_t hashBoard(const Board &b) {
        uint64_t hash = 0;
        for (size_t i = 0; i < 64; i++) {
            if (b.board[i].pieceType != PieceType::N_PIECES) {
                hash ^= getPieceHash(b.board[i], static_cast<Square>(i));
            }
        }
        hash ^= getCastleHash(b.currState.castlingState);

        hash ^= getTurnHash(b.currState.currentPlayer);

        bb epSquareBB = bitboard::setbitr(0ull, b.currState.enPassantSquare);

        if (b.currState.polyglotEnPassant) {
            bb pawnAttacks = b.currState.currentPlayer == WHITE ? 
                moveGen::blackPawnAttacks(epSquareBB, b.bitboards[b.currState.currentPlayer][PAWN]) :
                moveGen::whitePawnAttacks(epSquareBB, b.bitboards[b.currState.currentPlayer][PAWN]);
            if (pawnAttacks) {
                hash ^= getEnPassantHash(b.currState.enPassantSquare);
            }
        }

        return hash;
    }
}
