#include "../include/uci.hpp"
#include "../include/movegen.hpp"
#include "../include/board.hpp"
#include <memory>

int main() {
    moveGen::init();
    std::shared_ptr<Board> b = std::make_shared<Board>();
    uci::uciLoop(b);
    return 0;
}
