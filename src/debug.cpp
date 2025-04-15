#include "../include/debug.hpp"
#include "../include/colors.h"

#include <iostream>

namespace debug {
    void printMove(const Board &b, const Move &mv) {
        const TermColor::Modifier captureColor(TermColor::FG_RED);
        const TermColor::Modifier promotionColor(TermColor::FG_GREEN);
        const TermColor::Modifier castleColor(TermColor::FG_BLUE);
        const TermColor::Modifier defaultColor(TermColor::FG_DEFAULT);
        const TermColor::Modifier squareFromColor(TermColor::FG_MAGENTA);
        const TermColor::Modifier squareToColor(TermColor::FG_BLUE);

        std::cout << "[";
        std::cout << squareFromColor << squareWord[mv.from()];
        std::cout << defaultColor << " -> ";
        std::cout << squareToColor << squareWord[mv.to()] << defaultColor << "  ";

        std::cout << pieceCharacters[b.currState.currentPlayer][b.board[mv.from()].pieceType];
        if (mv.isCapture()) {
            std::cout << captureColor << "  ";
            std::cout << pieceCharacters[b.currState.currentPlayer][b.board[mv.to()].pieceType];
            std::cout << " ❌" << defaultColor;
        }
        if (mv.isPromotion()) {
            std::cout << promotionColor;
            std::cout << pieceCharacters[b.currState.currentPlayer][mv.promotionPiece()];
            std::cout << defaultColor;
        }
        if (mv.isCastle()) {
            std::cout << castleColor;
            if ((mv.move >> 12) == MoveFlag::CASTLE_QUEENSIDE) std::cout << "  O-O-O ";
            else std::cout << "  O-O ";
            std::cout << defaultColor;
        }
        std::cout << "]" << std::endl;
    }

    void printBoard(const Board &b) {
        for (int rank = 0; rank < 8; rank++) {
            for (int file = 0; file < 8; file++) {
                if (!file) std::cout << 8 - rank << "\t";
                std::cout << printPiece(b.board[rank * 8 + file]) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "\n\ta b c d e f g h\n" << std::endl;

        const GameState &currState = b.currState;

        std::cout << "Current Player "; 
        std::cout << (currState.currentPlayer == WHITE ? "WHITE" : "BLACK") << std::endl;

        if (currState.isInCheck) std::cout << "In Check" << std::endl;
        if (currState.enPassantSquare != N_SQUARES) 
            std::cout << "En passant square : " << squareWord[currState.enPassantSquare] << std::endl;

        if (currState.castlingState != 0) {
            std::cout << "Available castling : ";
            if (currState.castlingState & CASTLE_KING_WHITE) std::cout << "K";
            if (currState.castlingState & CASTLE_QUEEN_WHITE) std::cout << "Q";
            if (currState.castlingState & CASTLE_KING_BLACK) std::cout << "k";
            if (currState.castlingState & CASTLE_QUEEN_BLACK) std::cout << "q";
            std::cout << std::endl;
        }
    }

    void printBitboard(const bb &in) {
        for (int rank = 0; rank < 8; rank++) {
            for (int file = 0; file < 8; file++) {
                int square = rank * 8 + file;
                if (!file) std::cout << 8 - rank << "\t";
                std::cout << bitboard::getBit(in, (Square)(rank * 8 + file)) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "\n\ta b c d e f g h\n" << std::endl;
    }
}
