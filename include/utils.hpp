#pragma once

#include <string>
#include <unordered_map>

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

enum Square : int {
  A8, B8, C8, D8, E8, F8, G8, H8,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A1, B1, C1, D1, E1, F1, G1, H1,
  N_SQUARES,
  EMPTY_SQUARE = -1
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

enum class castlingRights : int {
    CASTLE_KING_WHITE = 1,
    CASTLE_QUEEN_WHITE = 2,
    CASTLE_KING_BLACK = 4,
    CASTLE_QUEEN_BLACK = 8
};

const std::string pieceCharacters[2][6] = {
    { u8"♟", u8"♞", u8"♝", u8"♜", u8"♛", u8"♚" },
    { u8"♙", u8"♘", u8"♗", u8"♖", u8"♕", u8"♔" }
};

const std::string squareWord[64] = {
    "A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8",
    "A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
    "A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
    "A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
    "A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
    "A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
    "A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
    "A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1",
};

const std::unordered_map<char, int> pieceNumberMap = {
        {'P', 0},
        {'N', 1},
        {'B', 2},
        {'R', 3},
        {'Q', 4},
        {'K', 5},
};
