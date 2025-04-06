#include "../include/board.hpp"
#include "../include/utils.hpp"
#include "../include/bitboard.hpp"
#include "../include/generatemoves.hpp"
#include "../include/colors.h"

#include <cctype>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <map>

void Board::printBoard() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            if (!file) std::cout << 8 - rank << "\t";
            std::cout << printPiece(this->board[rank * 8 + file]) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "\n\ta b c d e f g h\n" << std::endl;

    if (gameState.isInCheck) std::cout << "In Check" << std::endl;
    if (gameState.enPassantSquare != N_SQUARES) 
        std::cout << "En passant square : " << squareWord[gameState.enPassantSquare] << std::endl;

    if (gameState.castlingState != 0) {
        std::cout << "Available castling : ";
        if (gameState.castlingState & CASTLE_KING_WHITE) std::cout << "K";
        if (gameState.castlingState & CASTLE_QUEEN_WHITE) std::cout << "Q";
        if (gameState.castlingState & CASTLE_KING_BLACK) std::cout << "k";
        if (gameState.castlingState & CASTLE_QUEEN_BLACK) std::cout << "q";
        std::cout << std::endl;
    }
}

Board::Board(std::string FEN) {
    std::stringstream split1(FEN);
    std::string board;
    split1 >> board;

    std::cout << board << std::endl;

    std::stringstream rowsplit(board);
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

    std::string activeColor, castlingRights, enPassantSquareFEN;
    split1 >> activeColor >> castlingRights >> enPassantSquareFEN;

    if (activeColor == "b") gameState.currentPlayer = BLACK;
    else gameState.currentPlayer = WHITE;

    const std::map<int, int> maptocastle = {
        { 'Q', CASTLE_QUEEN_WHITE },
        { 'K', CASTLE_KING_WHITE},
        { 'q', CASTLE_QUEEN_BLACK},
        { 'k', CASTLE_QUEEN_BLACK},
    };

    gameState.castlingState = 0;
    if (castlingRights != "-") {
        for (auto c : castlingRights)
            gameState.castlingState |= maptocastle.at(c);
    }

    if (enPassantSquareFEN == "-") gameState.enPassantSquare = N_SQUARES;
    else gameState.enPassantSquare = wordSquare.at(enPassantSquareFEN);

    gameState.capturedPiece = {N_PIECES, N_COLORS};

    Color opps = static_cast<Color>(static_cast<int>(gameState.currentPlayer ^ 1));

    gameState.isInCheck = moveGen::isSquareAttacked(bitboards, 
            bitboard::getLsb(bitboards[gameState.currentPlayer][KING]), opps) != N_SQUARES;
}

inline std::string Board::printPiece(Piece p) { 
    if (p.pieceType == N_PIECES) return ".";
    return pieceCharacters[p.color][p.pieceType]; 
}


void Board::printMove(Board &b, Move &mv) {
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

    std::cout << pieceCharacters[b.gameState.currentPlayer][b.board[mv.from()].pieceType];
    if (mv.isCapture()) {
        std::cout << captureColor << "  ";
        std::cout << pieceCharacters[b.gameState.currentPlayer][b.board[mv.to()].pieceType];
        std::cout << " ❌" << defaultColor;
    }
    if (mv.isPromotion()) {
        std::cout << promotionColor;
        std::cout << pieceCharacters[b.gameState.currentPlayer][mv.promotionPiece()];
        std::cout << defaultColor;
    }
    if (mv.isCastle()) {
        std::cout << castleColor;
        if (mv.move == CASTLE_QUEENSIDE) std::cout << "  O-O-O ";
        else std::cout << "  O-O ";
        std::cout << defaultColor;
    }
    std::cout << "]" << std::endl;
}

Board::Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR") {}
