#include "../include/board.hpp"
#include "../include/generatemoves.hpp"
#include <vector>

#include <iostream>

void boardInitTest() {
    Board *b = new Board;
    b->printBoard();
}

void boardFENTest() {
    moveGen::init();
    Board *b  = new Board("4r1k1/8/1p6/2pP4/8/8/4B3/4K2R w K c6 0 1");
    b->printBoard();
}

void moveTest() {
    moveGen::init();
    Board *b  = new Board("4r1k1/8/1p6/2pP4/8/8/4B3/4K2R w K c6 0 1");
    b->printBoard();

    std::vector<Move> leg;
    moveGen::genPsuedoLegalMoves(*b, leg);

    std::cout << leg.size() << std::endl;

    for (auto &m : leg){
        Board::printMove(*b, m);
    }
}

int main() {
    moveTest();
}
