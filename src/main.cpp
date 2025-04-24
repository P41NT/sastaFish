#include "../include/uci.hpp"
#include "../include/movegen.hpp"
#include "../include/board.hpp"
#include "../include/zobrist.hpp"
#include "../include/ttable.hpp"
#include "../include/colors.hpp"

#include <iostream>

void printBanner() {
    const TermColor::Modifier bannerColor(TermColor::FG_BLUE);
    const TermColor::Modifier authorColor(TermColor::FG_MAGENTA);
    const TermColor::Modifier versionColor(TermColor::FG_DEFAULT);
    const TermColor::Modifier defaultColor(TermColor::FG_DEFAULT);

    std::string banner = R"(
                        __        _______      __  
       _________ ______/ /_____ _/ ____(_)____/ /_ 
      / ___/ __ `/ ___/ __/ __ `/ /_  / / ___/ __ \
     (__  ) /_/ (__  ) /_/ /_/ / __/ / (__  ) / / /
    /____/\__,_/____/\__/\__,_/_/   /_/____/_/ /_/
    )";

    std::string author = "Author:\tShawn Theo Moses ";
    std::string version = "Version:\t0.1.0";

    std::cout << bannerColor << banner << std::endl;
    std::cout << authorColor << author << std::endl;
    std::cout << defaultColor << versionColor << version << std::endl;
    std::cout << defaultColor << std::endl;
}

int main() {

    printBanner();

    moveGen::init();
    zobrist::init();
    Board *b = new Board();
    TTable *tt = new TTable();
    uci::uciLoop(*b, *tt);

    return 0;
}
