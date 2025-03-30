#include "../include/board.hpp"

void printBoardTest() {
    Board *b = new Board;
    b->printBoard();
}

void FENTest() {
    Board *b = new Board("8/6K1/1p3p2/n1P2pN1/3N2Pk/2P1p2P/2q1P1B1/8 w - - 0 1");
    b->printBoard();
}

void printMoveTest() {
    Board::Piece p{KNIGHT, BLACK};
    Board::Move m{p, E4, G8, false, false, false};
    Board::printMove(m);
}

int main() {
    // FENTest();
    // printBoardTest();
    printMoveTest();
}
