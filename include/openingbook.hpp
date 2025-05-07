#pragma once

#include <cstdint>
#include "board.hpp"
#include <vector>

namespace book {
    struct PolyglotEntry {
        uint64_t key;
        uint16_t move;
        uint16_t weight;
        uint32_t learn;
    };

    class Book {
        std::vector<PolyglotEntry> entries;
        public:

        Book(std::string &&filename);
        std::vector<PolyglotEntry> getEntries(uint64_t hash);
    };

    Move convertToMove(const PolyglotEntry& entry, const Board& board);
};
