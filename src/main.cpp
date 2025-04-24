#include "../include/uci.hpp"
#include "../include/movegen.hpp"
#include "../include/board.hpp"
#include "../include/zobrist.hpp"
#include "../include/ttable.hpp"

#include <iostream>

int main() {

    std::cout << "starting" << std::endl;

    moveGen::init();
    zobrist::init();
    Board *b = new Board();
    TTable *tt = new TTable();
    uci::uciLoop(*b, *tt);

    return 0;
}
