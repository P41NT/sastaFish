#include "../include/board.hpp"
#include "../include/utils.hpp"
#include "../include/bitboard.hpp"
#include "../include/generatemoves.hpp"

#include <cctype>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

void Board::printBoard() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            if (!file) std::cout << 8 - rank << "\t";
            std::cout << printPiece(this->board[rank * 8 + file]) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "\n\ta b c d e f g h\n" << std::endl;
}

Board::Board(std::string FEN) {
    std::stringstream split1(FEN);
    std::string board, remain;
    split1 >> board >> remain;

    std::cout << board << std::endl << remain << std::endl;

    std::stringstream rowsplit(FEN);
    std::string currRow;
    int rank = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            this->board[i * 8 + j] = Piece{N_PIECES, N_COLORS};
        }
    }

    while (!rowsplit.eof()) {
        std::getline(rowsplit, currRow, '/');

        int l = currRow.length();
        int file = 0;


        for (int i = 0; i < l; i++) {
            Square square = (Square)(rank * 8 + file);
            if (std::isalpha(currRow[i])) {
                int col = 0;

                if (std::islower(currRow[i])) {
                    col = 1;
                    currRow[i] -= 32;
                }

                int pie = pieceNumberMap.at(currRow[i]);
                bitboards[col][pie] = bitboard::setbit(bitboards[col][pie], square);
                bitboards[col][6] = bitboard::setbit(bitboards[col][6], square);

                // std::cout << pieceCharacters[pie][col] << std::endl;
                this->board[rank * 8 + file] = Piece{(PieceType)pie, (Color)col};

                file++;
            }
            else {
                int dig = currRow[i] - '0';
                file += dig;
            }
        }
        rank++;
    }

    // TODO : set this w.r.t FEN string smh
    gameState.castlingState = 0b1111;
    gameState.enPassantSquare = N_SQUARES;
    gameState.capturedPiece = {N_PIECES, N_COLORS};
    gameState.currentPlayer = WHITE;

    Color opps = static_cast<Color>(static_cast<int>(gameState.currentPlayer ^ 1));

    gameState.isInCheck = moveGen::isSquareAttacked(bitboards, 
            bitboard::getLsb(bitboards[gameState.currentPlayer][KING]), opps);
}

inline std::string Board::printPiece(Piece p) { 
    if (p.pieceType == N_PIECES) return ".";
    return pieceCharacters[p.color][p.pieceType]; 
}


void Board::printMove(Board &b, Move &mv) {
    std::cout << "[" << squareWord[mv.from()] << "->" << squareWord[mv.to()] << "\t" << 
        pieceCharacters[b.board[mv.from()].color][b.board[mv.to()].pieceType] 
        << (mv.isCapture() ? " X" : "") << (mv.isPromotion() ? " P" : "")<< "]" << std::endl;
}

Board::Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR") {}
