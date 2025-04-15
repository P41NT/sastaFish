#pragma once

#include "utils.hpp"
#include "bitboard.hpp"
#include "move.hpp"
#include <array>
#include <cstdint>
#include <stack>
#include <string>

struct Piece {
    PieceType pieceType;
    Color color;
    
    bool operator==(const Piece &b) const {
        return (b.pieceType == pieceType && b.color == color);
    }
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

    void makeMove(Move move);
    void unMakeMove(Move move);
    void unMakeMove();
};

