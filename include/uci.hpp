#pragma once

#include "board.hpp"
#include <memory>
#include <string>

namespace uci {
    static const std::string engineName = "sastaFish";
    static const std::string engineAuthor = "shobwq";

    void uciLoop(std::shared_ptr<Board> &b);
    void inputUci();
    void inputIsReady();
    void inputPosition(std::shared_ptr<Board> &b, const std::string command);
    void outputBestMove(std::shared_ptr<Board> b);
    void gop(std::shared_ptr<Board> b);
    Move parseMove(Board &b, const std::string &mv);

    void debug(std::shared_ptr<Board> b);
}
