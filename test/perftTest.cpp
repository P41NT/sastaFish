#include "../include/board.hpp"
#include "../include/movegen.hpp"
#include "../include/debug.hpp"
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

int perft(std::shared_ptr<Board> b, int depth, bool debug) {
    if (depth == 0) return 1;

    int answer = 0;
    std::vector<Move> legalMoves = moveGen::genLegalMoves(*b);

    if (legalMoves.size() == 0) return 1;


    for (auto &mv : legalMoves) {
        b->makeMove(mv);
        int moveAnswer = perft(b, depth - 1, false);
        answer += moveAnswer;
        b->unMakeMove();

        if (debug) {
            std::cout << mv.getUciString() << " " << moveAnswer << std::endl;
        }
    }

    if (debug) {
        std::cout << std::endl << answer << std::endl;
    }
    return answer;
}

Move getMoveFromUCI(std::shared_ptr<Board> b, const std::string &mv) {
    Square fromSquare = wordSquare.at(mv.substr(0, 2));
    Square toSquare = wordSquare.at(mv.substr(2, 2));

    if (mv.size() == 5) { 
        char promotion = mv[4]; 
        MoveFlag cap = (b->board[toSquare].pieceType != N_PIECES) ? CAPTURE : QUIET;
        switch (promotion) {
            case 'n':
            case 'N':
                return (Move(fromSquare, toSquare, cap | PROMOTE_N));
                break;
            case 'b':
            case 'B':
                return (Move(fromSquare, toSquare, cap | PROMOTE_B));
                break;
            case 'r':
            case 'R':
                return (Move(fromSquare, toSquare, cap | PROMOTE_R));
                break;
            case 'q':
            case 'Q':
                return (Move(fromSquare, toSquare, cap | PROMOTE_Q));
                break;
        }
    }

    if (b->board[toSquare].pieceType != N_PIECES) {
        return (Move(fromSquare, toSquare, MoveFlag::CAPTURE));
    }

    if (fromSquare == E1 && toSquare == G1 && b->board[fromSquare] == Piece{KING, WHITE}) 
        return (Move(fromSquare, toSquare, MoveFlag::CASTLE_KINGSIDE));
    if (fromSquare == E1 && toSquare == C1 && b->board[fromSquare] == Piece{KING, WHITE}) 
        return (Move(fromSquare, toSquare, MoveFlag::CASTLE_QUEENSIDE));
    if (fromSquare == E8 && toSquare == G8 && b->board[fromSquare] == Piece{KING, BLACK}) 
        return (Move(fromSquare, toSquare, MoveFlag::CASTLE_KINGSIDE));
    if (fromSquare == E8 && toSquare == C8 && b->board[fromSquare] == Piece{KING, BLACK}) 
        return (Move(fromSquare, toSquare, MoveFlag::CASTLE_QUEENSIDE));

    if (toSquare == b->currState.enPassantSquare && b->board[fromSquare].pieceType == PAWN) 
        return (Move(fromSquare, toSquare, EN_PASSANT));
    
    return (Move(fromSquare, toSquare));
}

int main(int argc, char** argv) {
    moveGen::init();

    auto b = std::make_shared<Board>();
    int depth = 1;

    std::string move;
    char option;
    std::string newFen;
    
    int total = 0;

    Move mv;

    int temp;

    int color, piece;

    while (true) {
        std::cout << ">> ";
        std::cin >> option;
        switch (option) {
            case 'd':
                std::cin >> temp;
                depth = temp;
                break;

            case 'b':
                debug::printBoard(*b);
                break;

            case 'm':
                for (auto &mv : moveGen::genLegalMoves(*b)) {
                    debug::printMove(*b, mv);
                }
                break;

            case 'n':
                std::cin >> move;
                mv = getMoveFromUCI(b, move);
                b->makeMove(mv);
                break;

            case 'u':
                b->unMakeMove();
                break;

            case 'f':
                std::getline(std::cin, newFen);
                b = std::make_shared<Board>(newFen);
                depth = 1;
                break;

            case 'p':
                perft(b, depth, true);
                break;

            case 'o':
                std::cin >> piece >> color;
                debug::printBitboard(b->bitboards[(Color)color][(PieceType)piece]);
        }
    }
}
