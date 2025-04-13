#include "../include/board.hpp"
#include "../include/generatemoves.hpp"
#include <vector>
#include <iostream>

void moveTest() {
    moveGen::init();
    Board *b  = new Board();
    b->printBoard();
    //
    // std::vector<Move> leg;
    // moveGen::genLegalMoves(*b, leg);
    // for (auto &m : leg){
    //     Board::printMove(*b, m);
    // }
}

int main() {
    moveTest();
}
