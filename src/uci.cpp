#include "../include/uci.hpp"
#include "../include/search.hpp"
#include "../include/debug.hpp"

#include <iostream>
#include <sstream>

namespace uci {
    void uciLoop(std::shared_ptr<Board> &b) {
        std::string command;

        while (getline(std::cin, command)) {
            if (command == "uci") inputUci();
            else if (command == "quit" || command == "stop") break;
            else if (command == "isready") inputIsReady();
            else if (command.rfind("position", 0) == 0) inputPosition(b, command);
            else if (command.rfind("go", 0) == 0) outputBestMove(b);
            else if (command.rfind("pog", 0) == 0) gop(b);
            else if (command == "debug") debug(b);
        }
    } 

    void inputUci() {
        std::cout << "id name " << engineName << std::endl;
        std::cout << "id author " << engineAuthor << std::endl;

        std::cout << "uciok" << std::endl;
    }

    void inputIsReady() {
        std::cout << "readyok" << std::endl;
    }

    void inputPosition(std::shared_ptr<Board> &b, const std::string command) {
        std::istringstream iss(command);

        std::string position;
        std::string option;
        iss >> position >> option;

        if (option == "startpos") {
            b->setFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        }
        else if (option == "fen") {
            std::string FEN; 
            int FENWords = 6;
            std::string FENWord;
            while (FENWords-- && iss >> FENWord) {
                FEN += FENWord + " ";
            }
            b->setFEN(FEN);
        }

        if (iss >> option && option == "moves") {
            std::string move;
            while (iss >> move) {
                b->makeMove(parseMove(*b, move));
            }
        }
    }

    void outputBestMove(std::shared_ptr<Board> b) {
        Move bestMove = search::bestMove(*b);
        std::cout << "bestmove " << bestMove.getUciString() << std::endl;
    }


    Move parseMove(Board &b, const std::string &mv) {
        Square fromSquare = wordSquare.at(mv.substr(0, 2));
        Square toSquare = wordSquare.at(mv.substr(2, 2));

        if (mv.size() == 5) { 
            char promotion = mv[4];
            MoveFlag cap = (b.board[toSquare].pieceType != N_PIECES) ? CAPTURE : QUIET;
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

        if (b.board[toSquare].pieceType != N_PIECES) {
            return (Move(fromSquare, toSquare, MoveFlag::CAPTURE));
        }

        if (fromSquare == E1 && toSquare == G1 && b.board[fromSquare] == Piece{KING, WHITE}) 
            return (Move(fromSquare, toSquare, MoveFlag::CASTLE_KINGSIDE));
        if (fromSquare == E1 && toSquare == C1 && b.board[fromSquare] == Piece{KING, WHITE}) 
            return (Move(fromSquare, toSquare, MoveFlag::CASTLE_QUEENSIDE));
        if (fromSquare == E8 && toSquare == G8 && b.board[fromSquare] == Piece{KING, BLACK}) 
            return (Move(fromSquare, toSquare, MoveFlag::CASTLE_KINGSIDE));
        if (fromSquare == E8 && toSquare == C8 && b.board[fromSquare] == Piece{KING, BLACK}) 
            return (Move(fromSquare, toSquare, MoveFlag::CASTLE_QUEENSIDE));

        if (toSquare == b.currState.enPassantSquare && b.board[fromSquare].pieceType == PAWN) 
            return (Move(fromSquare, toSquare, EN_PASSANT));
        
        return Move(fromSquare, toSquare);
    }

    void gop(std::shared_ptr<Board> b) {
        Move bestMove = search::bestMove(*b);
        std::cout << "bestmove " << bestMove.getUciString() << std::endl;
    }

    void debug(std::shared_ptr<Board> b) {
        debug::printBoard(*b);
    }
}
