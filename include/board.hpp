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
    uint64_t polyglotHash;
    Color currentPlayer;
    uint8_t castlingState;
    Square enPassantSquare;
    bool isInCheck;
    bool polyglotEnPassant;
};

template<typename T, size_t MaxSize>
class FastStack {
    T data[MaxSize];
    size_t _size = 0;

public:
    inline void push(const T& value) noexcept {
        data[_size++] = value;
    }
    inline void push(const T&& value) noexcept {
        data[_size++] = std::move(value);
    }

    template<typename... Args>
    inline void emplace(Args&&... args) noexcept {
        data[_size++] = T(std::forward<Args>(args)...);
    }

    inline void pop() noexcept {
        _size--;
    }

    [[nodiscard]] inline T& top() noexcept {
        return data[_size - 1];
    }

    [[nodiscard]] inline const T& top() const noexcept {
        return data[_size - 1];
    }

    [[nodiscard]] inline bool empty() const noexcept {
        return _size == 0;
    }

    [[nodiscard]] inline size_t size() const noexcept {
        return _size;
    }

    inline void clear() noexcept {
        _size = 0;
    }
};

class Board {
private:
    Square getKingSquare(Color color);
public: 
    //std::stack<Move> moves;
    //std::stack<Piece> captured;
    //std::stack<GameState> gameStates;

    FastStack<Move, 300> moves;
    FastStack<Piece, 300> captured;
    FastStack<GameState, 300> gameStates;

    //uint64_t polyglotHash;

    GameState currState;

    std::array<std::array<bb, 7>, 2> bitboards;
    std::array<Piece, 64> board;

    Board(std::string FEN);
    bool legalCheck(Move mv);
    Board();

    void makeMove(Move move);
    void unMakeMove();

    bool isKingSafe(Color color);

    void setFEN(std::string &FEN);
};

