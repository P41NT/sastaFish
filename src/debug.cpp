#include "../include/debug.hpp"
#include "../include/colors.hpp"
#include "../include/evaluation.hpp"

#include <iostream>
#include <memory>

namespace debug {
    void printMove(const Board &b, const Move &mv) {
        const TermColor::Modifier captureColor(TermColor::FG_RED);
        const TermColor::Modifier promotionColor(TermColor::FG_GREEN);
        const TermColor::Modifier castleColor(TermColor::FG_BLUE);
        const TermColor::Modifier defaultColor(TermColor::FG_DEFAULT);
        const TermColor::Modifier squareFromColor(TermColor::FG_MAGENTA);
        const TermColor::Modifier squareToColor(TermColor::FG_BLUE);

        std::cerr << "[";
        std::cerr << squareFromColor << squareWord[mv.from()];
        std::cerr << defaultColor << " -> ";
        std::cerr << squareToColor << squareWord[mv.to()] << defaultColor << "  ";

        std::cerr << pieceCharacters[b.currState.currentPlayer][b.board[mv.from()].pieceType];
        if (mv.isCapture()) {
            std::cerr << captureColor << "  ";
            std::cerr << pieceCharacters[b.currState.currentPlayer][b.board[mv.to()].pieceType];
            std::cerr << " ❌" << defaultColor;
        }
        if (mv.isPromotion()) {
            std::cerr << promotionColor;
            std::cerr << pieceCharacters[b.currState.currentPlayer][mv.promotionPiece()];
            std::cerr << defaultColor;
        }
        if (mv.isCastle()) {
            std::cerr << castleColor;
            if ((mv.move >> 12) == MoveFlag::CASTLE_QUEENSIDE) std::cerr << "  O-O-O ";
            else std::cerr << "  O-O ";
            std::cerr << defaultColor;
        }
        std::cerr << "]" << std::endl;
    }

    void printBoard(Board &b) {
        for (int rank = 0; rank < 8; rank++) {
            for (int file = 0; file < 8; file++) {
                if (!file) std::cerr << 8 - rank << "\t";
                std::cerr << printPiece(b.board[rank * 8 + file]) << " ";
            }
            std::cerr << std::endl;
        }
        std::cerr << "\n\ta b c d e f g h\n" << std::endl;

        const GameState &currState = b.currState;

        std::cerr << "Current Player "; 
        std::cerr << (currState.currentPlayer == WHITE ? "WHITE" : "BLACK") << std::endl;

        if (currState.isInCheck) std::cerr << "In Check" << std::endl;
        if (currState.enPassantSquare != N_SQUARES) 
            std::cerr << "En passant square : " << squareWord[currState.enPassantSquare] << std::endl;

        if (currState.castlingState != 0) {
            std::cerr << "Available castling : ";
            if (currState.castlingState & CASTLE_KING_WHITE)  std::cerr << "K";
            if (currState.castlingState & CASTLE_QUEEN_WHITE) std::cerr << "Q";
            if (currState.castlingState & CASTLE_KING_BLACK)  std::cerr << "k";
            if (currState.castlingState & CASTLE_QUEEN_BLACK) std::cerr << "q";
            std::cerr << std::endl;
        }

        std::cerr << "Zobrist Hash : " << b.zobristHash << std::endl;

        std::cerr << "Static Evaluation : " << eval::evaluateBoard(b) << std::endl;
    }

    void printBitboard(const bb &in) {
        for (int rank = 0; rank < 8; rank++) {
            for (int file = 0; file < 8; file++) {
                int square = rank * 8 + file;
                if (!file) std::cerr << 8 - rank << "\t";
                std::cerr << bitboard::getBit(in, (Square)(square)) << " ";
            }
            std::cerr << std::endl;
        }
        std::cerr << "\n\ta b c d e f g h\n" << std::endl;
    }
}
