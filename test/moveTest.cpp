#include "../include/board.hpp"
#include "../include/generatemoves.hpp"
#include <memory>

void testQuiet() {
    moveGen::init();
    std::shared_ptr<Board> b {std::make_shared<Board>("4r1k1/8/1p6/2pP4/8/8/4B3/4K2R w K c6 0 1")};
    b->printBoard();
    Move *mo = new Move(D5, C6, MoveFlag::EN_PASSANT);
    b->makeMove(*mo);
    b->printBoard();
    b->unMakeMove(*mo);
    b->printBoard();
}

int main() {
    testQuiet();
}
