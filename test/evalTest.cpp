#include "../include/evaluation.hpp"
#include "../include/movegen.hpp"
#include "../include/debug.hpp"
#include <iostream>
#include <memory>

int main(int argc, char** argv) {
    moveGen::init();
    std::string FEN = argv[1];
    std::cout << FEN << std::endl;
    auto b = std::make_shared<Board>(FEN);
    debug::printBoard(*b);
    int seeVal = eval::staticExchange(*b, C5);
    std::cout << seeVal << std::endl;
}
