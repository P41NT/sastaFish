#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>
#include <map>

enum Rank : int {
    RANK8,
    RANK7,
    RANK6,
    RANK5,
    RANK4,
    RANK3,
    RANK2,
    RANK1,
    N_RANKS
};

enum File: int {
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H,
    N_FILES
};

enum Square : uint8_t {
  A8, B8, C8, D8, E8, F8, G8, H8,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A1, B1, C1, D1, E1, F1, G1, H1,
  N_SQUARES,
};


enum Color : int {
    WHITE = 0,
    BLACK = 1,
    N_COLORS = 2
};

enum PieceType {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    N_PIECES
};

enum castlingRights : int {
    CASTLE_KING_WHITE = 1,
    CASTLE_QUEEN_WHITE = 2,
    CASTLE_KING_BLACK = 4,
    CASTLE_QUEEN_BLACK = 8
};

const std::string pieceCharacters[2][6] = {
    { u8"♟", u8"♞", u8"♝", u8"♜", u8"♛", u8"♚" },
    { u8"♙", u8"♘", u8"♗", u8"♖", u8"♕", u8"♔" }
};

const std::string pieceLetters[2][6] = {
    { "P", "N", "B", "R", "Q", "K" },
    { "p", "n", "b", "r", "q", "k" },
};

const std::string squareWord[64] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
};

const std::map<std::string, Square> wordSquare = {
    {"a8", A8}, {"b8", B8}, {"c8", C8}, {"d8", D8}, {"e8", E8}, {"f8", F8}, {"g8", G8}, {"h8", H8},
    {"a7", A7}, {"b7", B7}, {"c7", C7}, {"d7", D7}, {"e7", E7}, {"f7", F7}, {"g7", G7}, {"h7", H7},
    {"a6", A6}, {"b6", B6}, {"c6", C6}, {"d6", D6}, {"e6", E6}, {"f6", F6}, {"g6", G6}, {"h6", H6},
    {"a5", A5}, {"b5", B5}, {"c5", C5}, {"d5", D5}, {"e5", E5}, {"f5", F5}, {"g5", G5}, {"h5", H5},
    {"a4", A4}, {"b4", B4}, {"c4", C4}, {"d4", D4}, {"e4", E4}, {"f4", F4}, {"g4", G4}, {"h4", H4},
    {"a3", A3}, {"b3", B3}, {"c3", C3}, {"d3", D3}, {"e3", E3}, {"f3", F3}, {"g3", G3}, {"h3", H3},
    {"a2", A2}, {"b2", B2}, {"c2", C2}, {"d2", D2}, {"e2", E2}, {"f2", F2}, {"g2", G2}, {"h2", H2},
    {"a1", A1}, {"b1", B1}, {"c1", C1}, {"d1", D1}, {"e1", E1}, {"f1", F1}, {"g1", G1}, {"h1", H1},
};

const std::unordered_map<char, int> pieceNumberMap = {
        {'P', 0},
        {'N', 1},
        {'B', 2},
        {'R', 3},
        {'Q', 4},
        {'K', 5},
};

static const int inf = 1e9;
