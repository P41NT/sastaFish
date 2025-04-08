#include "../include/board.hpp"
#include "../include/utils.hpp"
#include "../include/bitboard.hpp"
#include "../include/generatemoves.hpp"
#include "../include/colors.h"

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <map>

void Board::printBoard() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            if (!file) std::cout << 8 - rank << "\t";
            // std::cout << this->board[rank * 8 + file].pieceType << " ";
            std::cout << printPiece(this->board[rank * 8 + file]) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "\n\ta b c d e f g h\n" << std::endl;

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

    if (activeColor == "b") currState.currentPlayer = BLACK;
    else currState.currentPlayer = WHITE;

    const std::map<int, int> maptocastle = {
        { 'Q', CASTLE_QUEEN_WHITE },
        { 'K', CASTLE_KING_WHITE},
        { 'q', CASTLE_QUEEN_BLACK},
        { 'k', CASTLE_QUEEN_BLACK},
    };

    currState.castlingState = 0;
    if (castlingRights != "-") {
        for (auto c : castlingRights)
            currState.castlingState |= maptocastle.at(c);
    }

    if (enPassantSquareFEN == "-") currState.enPassantSquare = N_SQUARES;
    else currState.enPassantSquare = wordSquare.at(enPassantSquareFEN);

    this->captured.push({N_PIECES, N_COLORS});

    const Color opps = static_cast<Color>(static_cast<int>(currState.currentPlayer ^ 1));

    currState.isInCheck = moveGen::isSquareAttacked(bitboards, 
            bitboard::getLsb(bitboards[currState.currentPlayer][KING]), opps) != N_SQUARES;
}

inline std::string Board::printPiece(Piece p) { 
    if (p.pieceType == N_PIECES) return ".";
    return pieceCharacters[p.color][p.pieceType]; 
}

void Board::makeMove(Move mv) {
    const Piece fromPiece = board[mv.from()];
    
    // updated the from piece bitboards
    bitboards[fromPiece.color][fromPiece.pieceType] = bitboard::toggleBit(
            bitboards[fromPiece.color][fromPiece.pieceType], mv.from());
    bitboards[fromPiece.color][fromPiece.pieceType] = bitboard::toggleBit(
            bitboards[fromPiece.color][fromPiece.pieceType], mv.to());

    // stored the capturedSquare and capturePiece
    Square captureSquare = mv.to();

    if (mv.isEnPassant()) {
        if (currState.currentPlayer == WHITE) captureSquare = (Square)((int)captureSquare + 8);
        else captureSquare = (Square)((int)captureSquare - 8);
    }

    const Piece capturePiece = board[captureSquare];

    std::cout << "captureSquare " << squareWord[captureSquare] << std::endl;

    // updated the board of from and to
    board[mv.from()] = Piece{N_PIECES, N_COLORS};
    board[mv.to()] = fromPiece;

    if (mv.isCapture()) {
        bitboards[capturePiece.color][capturePiece.pieceType] = bitboard::toggleBit(
                bitboards[capturePiece.color][capturePiece.pieceType], captureSquare);
        board[captureSquare] = Piece{N_PIECES, N_COLORS};
        this->captured.push(capturePiece);
    }
    if (mv.isPromotion()) {
        const Piece toPromote = Piece{ mv.promotionPiece(), currState.currentPlayer };
        bitboards[fromPiece.color][fromPiece.pieceType] = bitboard::toggleBit(
                bitboards[fromPiece.color][fromPiece.pieceType], mv.to());
        bitboards[toPromote.color][toPromote.pieceType] = bitboard::toggleBit(
                bitboards[toPromote.color][toPromote.pieceType], mv.to());
        board[mv.to()] = toPromote;
    }

    static const Square rookFromSquareK = static_cast<Square>(static_cast<int>(mv.from() + 3));
    static const Square rookToSquareK = static_cast<Square>(static_cast<int>(mv.to() - 1));
    static const Square rookFromSquareQ = static_cast<Square>(static_cast<int>(mv.from() + 3));
    static const Square rookToSquareQ = static_cast<Square>(static_cast<int>(mv.to() - 1));

    if (mv.isCastle()) {
        switch (mv.move >> 12) {
            case CASTLE_KINGSIDE:
                bitboards[fromPiece.color][ROOK] = bitboard::toggleBit(
                        bitboards[fromPiece.color][fromPiece.pieceType], rookFromSquareK);
                bitboards[fromPiece.color][ROOK] = bitboard::toggleBit(
                        bitboards[fromPiece.color][fromPiece.pieceType], rookToSquareK);
                board[rookFromSquareK] = {N_PIECES, N_COLORS};
                board[rookToSquareK] = {ROOK, fromPiece.color};
                break;

            case CASTLE_QUEENSIDE:
                bitboards[fromPiece.color][ROOK] = bitboard::toggleBit(
                        bitboards[fromPiece.color][fromPiece.pieceType], rookFromSquareQ);
                bitboards[fromPiece.color][ROOK] = bitboard::toggleBit(
                        bitboards[fromPiece.color][fromPiece.pieceType], rookToSquareQ);
                board[rookFromSquareQ] = {N_PIECES, N_COLORS};
                board[rookToSquareK] = {ROOK, fromPiece.color};
                break;
        }
    }

    gameStates.push(currState);

    switch (fromPiece.pieceType) {
        case KING:
            switch(fromPiece.color) {
                case WHITE: currState.castlingState &= ~(CASTLE_KING_WHITE | CASTLE_QUEEN_WHITE); break;
                case BLACK: currState.castlingState &= ~(CASTLE_KING_BLACK | CASTLE_QUEEN_BLACK); break;
                default: break;
            }
            break;
        case ROOK:
            switch (mv.from()) {
                case H1: currState.castlingState &= ~(CASTLE_KING_WHITE); break;
                case A1: currState.castlingState &= ~(CASTLE_QUEEN_WHITE); break;
                case H8: currState.castlingState &= ~(CASTLE_KING_BLACK); break;
                case A8: currState.castlingState &= ~(CASTLE_QUEEN_BLACK); break;
                default: break;
            }
            break;
        default: break;
    }


    currState.currentPlayer = static_cast<Color>(static_cast<int>(currState.currentPlayer) ^ 1);
    currState.enPassantSquare = N_SQUARES;

    const Square oppKingSquare = bitboard::getLsb(bitboards[currState.currentPlayer][KING]);
    currState.isInCheck = (moveGen::isSquareAttacked(bitboards, oppKingSquare, fromPiece.color) != N_SQUARES);
}

void Board::unMakeMove(Move mv) {
    currState = gameStates.top();
    gameStates.pop();
    currState.currentPlayer = static_cast<Color>(static_cast<int>(currState.currentPlayer) ^ 1);

    const Piece fromPiece = board[mv.to()];
    
    // updated the from piece bitboards
    bitboards[fromPiece.color][fromPiece.pieceType] = bitboard::toggleBit(
            bitboards[fromPiece.color][fromPiece.pieceType], mv.from());
    bitboards[fromPiece.color][fromPiece.pieceType] = bitboard::toggleBit(
            bitboards[fromPiece.color][fromPiece.pieceType], mv.to());

    std::cout << squareWord[mv.to()] << std::endl;

    // updated the board of from and to
    board[mv.from()] = fromPiece;
    board[mv.to()] = {N_PIECES, N_COLORS};

    if (mv.isCapture()) {
        Square captureSquare = mv.isEnPassant() ? currState.enPassantSquare : mv.to();
        if (mv.isEnPassant()) {
            if (currState.currentPlayer == WHITE) captureSquare = (Square)((int)captureSquare - 8);
            else captureSquare = (Square)((int)captureSquare + 8);
        }
        const Piece capturePiece = captured.top();
        captured.pop();
        bitboards[capturePiece.color][capturePiece.pieceType] = bitboard::toggleBit(
                bitboards[capturePiece.color][capturePiece.pieceType], captureSquare);
        board[captureSquare] = capturePiece;
    }
    if (mv.isPromotion()) {
        const Piece toPromote = Piece{ mv.promotionPiece(), currState.currentPlayer };
        bitboards[fromPiece.color][fromPiece.pieceType] = bitboard::toggleBit(
                bitboards[fromPiece.color][fromPiece.pieceType], mv.to());
        bitboards[toPromote.color][toPromote.pieceType] = bitboard::toggleBit(
                bitboards[toPromote.color][toPromote.pieceType], mv.to());
    }

    static const Square rookFromSquareK = static_cast<Square>(static_cast<int>(mv.from() + 3));
    static const Square rookToSquareK = static_cast<Square>(static_cast<int>(mv.to() - 1));
    static const Square rookFromSquareQ = static_cast<Square>(static_cast<int>(mv.from() + 3));
    static const Square rookToSquareQ = static_cast<Square>(static_cast<int>(mv.to() - 1));

    if (mv.isCastle()) {
        switch (mv.move >> 12) {
            case CASTLE_KINGSIDE:
                bitboards[fromPiece.color][ROOK] = bitboard::toggleBit(
                        bitboards[fromPiece.color][fromPiece.pieceType], rookFromSquareK);
                bitboards[fromPiece.color][ROOK] = bitboard::toggleBit(
                        bitboards[fromPiece.color][fromPiece.pieceType], rookToSquareK);
                board[rookToSquareK] = {N_PIECES, N_COLORS};
                board[rookFromSquareK] = {ROOK, fromPiece.color};
                break;

            case CASTLE_QUEENSIDE:
                bitboards[fromPiece.color][ROOK] = bitboard::toggleBit(
                        bitboards[fromPiece.color][fromPiece.pieceType], rookFromSquareQ);
                bitboards[fromPiece.color][ROOK] = bitboard::toggleBit(
                        bitboards[fromPiece.color][fromPiece.pieceType], rookToSquareQ);
                board[rookToSquareQ] = {N_PIECES, N_COLORS};
                board[rookFromSquareK] = {ROOK, fromPiece.color};
                break;
        }
    }
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
        if (mv.move == CASTLE_QUEENSIDE) std::cout << "  O-O-O ";
        else std::cout << "  O-O ";
        std::cout << defaultColor;
    }
    std::cout << "]" << std::endl;
}

Board::Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR") {}
