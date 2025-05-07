#include "../include/openingbook.hpp"
#include "../include/board.hpp"

#include <cstdint>
#include <iostream>
#include <ios>
#include <algorithm>
#include <fstream>

uint16_t read16BigEndian(const char* buffer) {
    return (static_cast<uint8_t>(buffer[0]) << 8) |
            static_cast<uint8_t>(buffer[1]);
}

uint32_t read32BigEndian(const char* buffer) {
    return (static_cast<uint8_t>(buffer[0]) << 24) |
           (static_cast<uint8_t>(buffer[1]) << 16) |
           (static_cast<uint8_t>(buffer[2]) << 8)  |
            static_cast<uint8_t>(buffer[3]);
}

uint64_t read64BigEndian(const char* buffer) {
    uint64_t hi = read32BigEndian(buffer);
    uint64_t lo = read32BigEndian(buffer + 4);
    return (hi << 32) | lo;
}

namespace book {
    Book::Book(std::string &&filename) {
        std::ifstream bookFile(filename, std::ios::in | std::ios::binary);

        bookFile.seekg(0, std::ios::end);
        std::streamsize bookSize = bookFile.tellg();
        bookFile.seekg(0, std::ios::beg);

        size_t numEntries = static_cast<size_t>(bookSize) / sizeof(PolyglotEntry);
        this->entries = std::vector<PolyglotEntry>(numEntries);

        char buffer[16];
        for (auto &entry : entries) {
            bookFile.read(buffer, 16);
            entry.key    = read64BigEndian(buffer);
            entry.move   = read16BigEndian(buffer + 8);
            entry.weight = read16BigEndian(buffer + 10);
            entry.learn  = read32BigEndian(buffer + 12);
        }
        bookFile.close();
    }

    std::vector<PolyglotEntry> Book::getEntries(uint64_t hash) {
        PolyglotEntry checker = {hash, 0, 0, 0};

        auto compare = [](const PolyglotEntry &a, const PolyglotEntry &b) {
            return a.key < b.key;
        };

        std::vector<PolyglotEntry> result;

        auto lower = std::lower_bound(entries.begin(), entries.end(), checker, compare);
        auto higher = std::upper_bound(entries.begin(), entries.end(), checker, compare);

        for (auto it = lower; it != higher; ++it) result.push_back(*it);

        return result;
    }

    Move convertToMove(const PolyglotEntry &entry, const Board &b) {
        int toFile = entry.move & (0b111);
        int toRow = 7 - ((entry.move >> 3) & (0b111));
        int fromFile = (entry.move >> 6) & (0b111);
        int fromRow = 7 - ((entry.move >> 9) & (0b111));
        int promotionPiece = (entry.move >> 12) & (0b111);
        
        Square fromSquare = static_cast<Square>((fromRow * 8) + fromFile);
        Square toSquare = static_cast<Square>((toRow * 8) + toFile);

        if (promotionPiece != 0) {
            MoveFlag cap = (b.board[toSquare].pieceType != N_PIECES) ? CAPTURE : QUIET;
            switch (promotionPiece) {
                case 1:
                    return (Move(fromSquare, toSquare, cap | PROMOTE_N));
                    break;
                case 2:
                    return (Move(fromSquare, toSquare, cap | PROMOTE_B));
                    break;
                case 3:
                    return (Move(fromSquare, toSquare, cap | PROMOTE_R));
                    break;
                case 4:
                    return (Move(fromSquare, toSquare, cap | PROMOTE_Q));
                    break;
            }
        }

        if (b.board[toSquare].pieceType != N_PIECES) {
            return (Move(fromSquare, toSquare, MoveFlag::CAPTURE));
        }

        if (fromSquare == E1 && toSquare == H1 && b.board[fromSquare] == Piece{KING, WHITE}) 
            return (Move(fromSquare, toSquare, MoveFlag::CASTLE_KINGSIDE));
        if (fromSquare == E1 && toSquare == A1 && b.board[fromSquare] == Piece{KING, WHITE}) 
            return (Move(fromSquare, toSquare, MoveFlag::CASTLE_QUEENSIDE));
        if (fromSquare == E8 && toSquare == H8 && b.board[fromSquare] == Piece{KING, BLACK}) 
            return (Move(fromSquare, toSquare, MoveFlag::CASTLE_KINGSIDE));
        if (fromSquare == E8 && toSquare == A8 && b.board[fromSquare] == Piece{KING, BLACK}) 
            return (Move(fromSquare, toSquare, MoveFlag::CASTLE_QUEENSIDE));

        if (b.board[fromSquare].pieceType == PAWN) {
            if (b.currState.currentPlayer == WHITE && toSquare == fromSquare - 16) 
                return (Move(fromSquare, toSquare, DOUBLE_PUSH));
            if (b.currState.currentPlayer == BLACK && toSquare == fromSquare + 16) 
                return (Move(fromSquare, toSquare, DOUBLE_PUSH));
        }

        if (toSquare == b.currState.enPassantSquare && b.board[fromSquare].pieceType == PAWN) 
            return (Move(fromSquare, toSquare, EN_PASSANT));
        
        return Move(fromSquare, toSquare);
    }
}
