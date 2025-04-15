#include "../include/board.hpp"
#include "../include/movegen.hpp"
#include "../include/debug.hpp"

void moveTest() {
    moveGen::init();
    Board *b = new Board("r3k2r/p1ppq1b1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    debug::printBoard(*b);

    b->makeMove(Move(E5, F7));
    b->makeMove(Move(B6, C8));
    b->makeMove(Move(F7, H8, CAPTURE));
    debug::printBoard(*b);
    for (auto &mv : moveGen::genLegalMoves(*b)) 
        debug::printMove(*b, mv);
    b->unMakeMove();
    debug::printBoard(*b);
    for (auto &mv : moveGen::genLegalMoves(*b)) 
        debug::printMove(*b, mv);

}

int main() {
    moveTest();
}
