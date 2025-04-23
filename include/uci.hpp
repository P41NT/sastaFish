#pragma once

#include "board.hpp"
#include "ttable.hpp"

#include <memory>
#include <string>

namespace uci {
    static const std::string engineName = "sastaFish";
    static const std::string engineAuthor = "shobwq";

    void uciLoop(std::shared_ptr<Board> &b, std::shared_ptr<TTable> &tt);
    void inputUci();
    void inputIsReady();
    void inputPosition(std::shared_ptr<Board> &b, const std::string command);
    void outputBestMove(std::shared_ptr<Board> b, std::shared_ptr<TTable> tt);
    Move parseMove(Board &b, const std::string &mv);

    void debug(std::shared_ptr<Board> b);
}
