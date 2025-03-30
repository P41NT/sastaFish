#pragma once

#include "utils.hpp"
#include "bitboard.hpp"
#include <array>
#include <string>
#include <vector>


class Board {
public:
    struct Piece {
        PieceType piece;
        Color color;
    };

    struct GameState {
        uint8_t castlingState;
        Square enPassantSquare;
        Color currentPlayer;
        bool isInCheck;
    };

    struct Move {
        Piece piece;
        Square from;
        Square to;

        bool isCapture;
        bool isPromotion;

        GameState oldState;
        GameState newState;
    };
    GameState gameState;
    std::array<std::array<bb, 7>, 2> bitboards;

    Board(std::string FEN);
    Board();
    ~Board();

    std::vector<Move> psuedoLegalMoves();
    std::vector<Move> legalMoves();

    void makeMove(Move move);
    void unMakeMove(Move move);

    void printBoard();
    static void printMove(Move &mv);
};
