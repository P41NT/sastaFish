#include "../include/board.hpp"
#include "../include/utils.hpp"
#include "../include/bitboard.hpp"

#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

void Board::printBoard() {
    std::string board[8][8];

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board[i][j] = ".";
        }
    }

    for (int c = 0; c < 2; c++) {
        for (int p = 0; p < 6; p++) {
            bb currBitboard = this->bitboards[c][p];
            std::string letter = pieceCharacters[c][p];
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    Square sq = (Square)(i * 8 + j);
                    if (bitboard::getBit(currBitboard, sq)) {
                        board[i][j] = letter;
                    }
                }
            }
        }
    }

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            if (!file) std::cout << 8 - rank << "\t";
            std::cout << board[rank][file] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "\n\ta b c d e f g h\n" << std::endl;
}

Board::Board(std::string FEN) {
    std::stringstream split1(FEN);
    std::string board, remain;
    split1 >> board >> remain;

    std::stringstream rowsplit(FEN);
    std::string currRow;
    int rank = 0;

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

                file++;
            }
            else {
                int dig = currRow[i] - '0';
                file += dig;
            }
        }
        rank++;
    }
}

void Board::printMove(Move &mv) {
    std::cout << "[" << squareWord[mv.from] << "->" << squareWord[mv.to] << "\t" << 
        pieceCharacters[mv.piece.color][mv.piece.piece] 
        << (mv.isCapture ? " X" : "") << (mv.isPromotion ? " P" : "")<< "]" << std::endl;
}

Board::Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w") {}
