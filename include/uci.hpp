#pragma once

#include "board.hpp"
#include "openingbook.hpp"
#include "ttable.hpp"
#include "../include/debug.hpp"

#include <string>

namespace uci {
    static const std::string engineName = "sastaFish";
    static const std::string engineAuthor = "shobwq";

    void uciLoop(Board &b, TTable &tt, RepetitionTable &rt, book::Book &bk);
    void inputUci();
    void inputIsReady();
    void inputPosition(Board &b, const std::string &command, RepetitionTable &rt);
    void outputBestMove(Board &b, TTable &tt, RepetitionTable &rt, book::Book &bk, std::string &command);
    Move parseMove(Board &b, const std::string &mv);

    inline void debug(Board &b) {
        debug::printBoard(b);
    }
}
