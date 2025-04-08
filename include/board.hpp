#pragma once

#include "utils.hpp"
#include "bitboard.hpp"
#include "move.hpp"
#include <array>
#include <cstdint>
#include <stack>
#include <string>
#include <vector>

struct Piece {
    PieceType pieceType;
    Color color;
};

struct GameState {
    uint8_t castlingState;
    Square enPassantSquare;
    Color currentPlayer;
    bool isInCheck;
};

class Board {
public: 
    std::stack<Move> moves;
    std::stack<Piece> captured;
    std::stack<GameState> gameStates;

    GameState currState;

    std::array<std::array<bb, 7>, 2> bitboards;
    std::array<Piece, 64> board;

    Board(std::string FEN);
    Board();

    std::vector<Move> psuedoLegalMoves();
    std::vector<Move> legalMoves();

    inline std::string printPiece(Piece p);

    void makeMove(Move move);
    void unMakeMove(Move move);

    void printBoard();
    static void printMove(Board &b, Move &mv);
};

