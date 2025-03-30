#include "../include/generatemoves.hpp"
#include "../include/board.hpp"

void testPsueodLegal() {
    moveGen::init();
    Board *b = new Board("2r5/1P2N3/k2P4/5bP1/8/3R3B/4K3/8");
    std::vector<Board::Move> psuedoLegalMoves;
    moveGen::genPsuedoLegalMoves(*b, psuedoLegalMoves);

    b->printBoard();

    for (auto move : psuedoLegalMoves) {
        Board::printMove(move);
    }
}

int main() {
    testPsueodLegal();
}
