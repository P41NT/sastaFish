#include "../include/generatemoves.hpp"
#include "../include/board.hpp"
#include "../include/utils.hpp"
#include <iostream>

void testisSquareAttacked() {
    moveGen::init();

    Board *b = new Board("8/8/4r3/8/2B5/8/8/8");
    b->printBoard();

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Square sq = (Square)(i * 8 + j);
            if (moveGen::isSquareAttacked(b->bitboards, sq, Color::WHITE))
                std::cout << "#" << " ";
            else std::cout << "." << " ";
        }
        std::cout << std::endl;
    }
}

void testSomething() {
    moveGen::init();
    bb testing = moveGen::horizontalMask[RANK1];
    bitboard::printBitboard(testing);
}

void testPsuedoLegal() {
    moveGen::init();
    Board *b = new Board("2r5/1P2N3/k2P4/5bP1/8/3R3B/8/8");
    // b->gameState.currentPlayer = BLACK;
    b->printBoard();

    std::vector<Board::Move> moves;
    moveGen::genPsuedoLegalMoves(*b, moves);
    for (auto &move : moves) {
        Board::printMove(move);
    }
}

int main() {
    // testisSquareAttacked();
    // testSomething();
    testPsuedoLegal();
}
