#include "../include/uci.hpp"
#include "../include/movegen.hpp"
#include "../include/board.hpp"
#include "../include/ttable.hpp"
#include "../include/colors.hpp"
#include "../include/openingbook.hpp"

#include <iostream>

void printBanner() {
    const TermColor::Modifier bannerColor(TermColor::FG_BLUE);
    const TermColor::Modifier authorColor(TermColor::FG_CYAN);
    const TermColor::Modifier versionColor(TermColor::FG_DEFAULT);
    const TermColor::Modifier defaultColor(TermColor::FG_DEFAULT);

    std::string banner = R"(
                        __        _______      __  
       _________ ______/ /_____ _/ ____(_)____/ /_ 
      / ___/ __ `/ ___/ __/ __ `/ /_  / / ___/ __ \
     (__  ) /_/ (__  ) /_/ /_/ / __/ / (__  ) / / /
    /____/\__,_/____/\__/\__,_/_/   /_/____/_/ /_/
    )";

    std::string author = "Author:\t\tShawn Theo Moses ";
    std::string version = "Version:\t0.3.0";

    std::cout << bannerColor << banner << std::endl;
    std::cout << authorColor << author << std::endl;
    std::cout << defaultColor << versionColor << version << std::endl;
    std::cout << defaultColor << std::endl;
}

int main() {
    printBanner();

    // freopen("log.txt", "w", stderr);

    moveGen::init();

    Board *b = new Board();

    TTable *tt = new TTable();
    RepetitionTable *rt = new RepetitionTable();
    openingbook::Book *bk = new openingbook::Book("/home/shobwq/Projects/sastaFish/data/Human.bin");

    uci::uciLoop(*b, *tt, *rt, *bk);

    return 0;
}
