#include "../include/generatemoves.hpp"
#include "../include/board.hpp"
#include <vector>

void testPsuedoLegal() {
    moveGen::init();
    Board *b = new Board("2r5/1P2N3/k2P4/5bP1/8/3R3B/4K3/8");
    std::vector<Board::Move> psuedoLegalMoves;
    moveGen::genPsuedoLegalMoves(*b, psuedoLegalMoves);

    b->printBoard();

    for (auto move : psuedoLegalMoves) { Board::printMove(move); }
}

void testLegal() {
    moveGen::init();

    std::string test[2] = {
        // "3rk3/8/8/3R4/8/8/3K4/8"
        // "3rk3/8/8/6R1/8/8/3K4/8",
        "3rk3/8/8/B5R1/8/8/3K4/8"
    };

    for (int i = 0; i < 1; i++) {
        Board *b = new Board(test[i]);
        b->gameState.isInCheck = true;

        std::vector<Board::Move> legalMoves;
        moveGen::genLegalMoves(*b, legalMoves);

        b->printBoard();
        for (auto move : legalMoves) { Board::printMove(move); }
    }
}

void rookMaskTest() {
    moveGen::init();

    bb smth = bitboard::setbit(0ull, D8);
    bb gen = moveGen::genRookMask(smth, D2);

    bitboard::printBitboard(gen);
}

int main() {
    // rookMaskTest();
    testLegal();
}
