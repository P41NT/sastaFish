#include "../include/board.hpp"
#include "../include/movegen.hpp"
#include "../include/debug.hpp"
#include "../include/search.hpp"

#include <iostream>

void moveTest() {
    moveGen::init();
    // Board *b = new Board("r3k2r/p1ppq1b1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    Board *b = new Board();
    debug::printBoard(*b);

    // std::cout << search::alphaBetaSearch(*b, -inf, inf, 2) << std::endl;
    // std::cout << eval::evaluateBoard(*b) << std::endl;
    std::cout << search::bestMove(*b, 5).getUciString() << std::endl;
}

int main() {
    moveTest();
}
