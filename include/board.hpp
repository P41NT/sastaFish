#pragma once

#include "utils.hpp"
#include "bitboard.hpp"
#include "move.hpp"
#include <array>
#include <string>
#include <vector>

struct Piece {
    PieceType pieceType;
    Color color;
};

class Board {
public:
    class GameState {
        public:
        uint8_t castlingState;
        Square enPassantSquare;
        Color currentPlayer;
        bool isInCheck;
        Piece capturedPiece;
        Move lastMove;
    };

    GameState gameState;
    std::array<std::array<bb, 7>, 2> bitboards;
    std::array<Piece, 64> board;

    Board(std::string FEN);
    Board();
    ~Board();

    std::vector<Move> psuedoLegalMoves();
    std::vector<Move> legalMoves();

    inline std::string printPiece(Piece p);

    void makeMove(Move move);
    void unMakeMove(Move move);

    void printBoard();
    static void printMove(Board &b, Move &mv);
};

