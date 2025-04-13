#include "../include/board.hpp"
#include "../include/generatemoves.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

int perft(std::shared_ptr<Board> b, int depth, bool debug) {
    if (depth == 0) return 1;

    int answer = 0;
    std::vector<Move> legalMoves;
    moveGen::genLegalMoves(*b, legalMoves);

    for (auto &mv : legalMoves) {
        b->makeMove(mv);
        answer += perft(b, depth - 1, debug);
        b->unMakeMove(mv);
    }

    return answer;
}

int main(int argc, char** argv) {
    moveGen::init();

    std::string initialFen = argv[2];
    std::shared_ptr<Board> b = std::make_shared<Board>(initialFen);

    std::vector<std::string> moves(argc - 3);
    for (int i = 3; i < argc; i++) { moves[i - 3] = (argv[i]); }

    for (auto &mv : moves) {
        Square fromSquare = wordSquare.at(mv.substr(0, 2));
        Square toSquare = wordSquare.at(mv.substr(2, 2));

        if (mv.size() == 5) { 
            char promotion = mv[4]; 
            MoveFlag cap = (b->board[toSquare].pieceType != N_PIECES) ? CAPTURE : QUIET;
            switch (promotion) {
                case 'n':
                case 'N':
                    b->makeMove(Move(fromSquare, toSquare, cap | PROMOTE_N));
                    break;
                case 'b':
                case 'B':
                    b->makeMove(Move(fromSquare, toSquare, cap | PROMOTE_B));
                    break;
                case 'r':
                case 'R':
                    b->makeMove(Move(fromSquare, toSquare, cap | PROMOTE_R));
                    break;
                case 'q':
                case 'Q':
                    b->makeMove(Move(fromSquare, toSquare, cap | PROMOTE_Q));
                    break;
            }
            continue;
        }

        if (b->board[toSquare].pieceType != N_PIECES) {
            std::cout << mv << " " << b->board[toSquare].pieceType << std::endl;
            b->makeMove(Move(fromSquare, toSquare, MoveFlag::CAPTURE));
            continue;
        }

        if (fromSquare == E1 && toSquare == G1 && b->board[fromSquare] == Piece{KING, WHITE}) {
            b->makeMove(Move(fromSquare, toSquare, MoveFlag::CASTLE_KINGSIDE));
            continue;
        }
        if (fromSquare == E1 && toSquare == C1 && b->board[fromSquare] == Piece{KING, WHITE}) {
            b->makeMove(Move(fromSquare, toSquare, MoveFlag::CASTLE_QUEENSIDE));
            continue;
        }
        if (fromSquare == E8 && toSquare == G8 && b->board[fromSquare] == Piece{KING, BLACK}) {
            b->makeMove(Move(fromSquare, toSquare, MoveFlag::CASTLE_KINGSIDE));
            continue;
        }
        if (fromSquare == E8 && toSquare == C8 && b->board[fromSquare] == Piece{KING, BLACK}) {
            b->makeMove(Move(fromSquare, toSquare, MoveFlag::CASTLE_QUEENSIDE));
            continue;
        }

        if (toSquare == b->currState.enPassantSquare && b->board[fromSquare].pieceType == PAWN) {
            b->makeMove(Move(fromSquare, toSquare, EN_PASSANT));
            continue;
        }
        
        b->makeMove(Move(fromSquare, toSquare));
    }
    int depth = std::stoi(argv[1]);

    std::vector<Move> legalMoves;
    moveGen::genLegalMoves(*b, legalMoves);

    int total = 0;
    for (auto &mv : legalMoves) {
        b->makeMove(mv);
        int moveAnswer = perft(b, depth - 1, false);
        b->unMakeMove(mv);
        total += moveAnswer;
        std::cout << mv.getUciString() << " " << moveAnswer << std::endl;
    }

    std::cout << std::endl << total << std::endl;
}
