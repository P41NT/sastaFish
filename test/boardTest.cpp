#include "../include/board.hpp"

void boardInitTest() {
    Board *b = new Board;
    b->printBoard();
}

int main() {
    boardInitTest();
}
