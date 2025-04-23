#include "../include/uci.hpp"
#include "../include/movegen.hpp"
#include "../include/board.hpp"
#include "../include/zobrist.hpp"
#include "../include/ttable.hpp"
#include <memory>

int main() {
    moveGen::init();
    zobrist::init();
    std::shared_ptr<Board> b = std::make_shared<Board>();
    std::shared_ptr<TTable> tt = std::make_shared<TTable>();
    uci::uciLoop(b, tt);
    return 0;
}
