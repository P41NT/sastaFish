#include "../include/uci.hpp"
#include "../include/search.hpp"
#include "../include/colors.hpp"
#include "../include/openingbook.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <chrono>

namespace uci {
    std::stack<uint64_t> initialStack;

    void uciLoop(Board &b, TTable &tt, RepetitionTable &rt, openingbook::Book &bk) {
        const TermColor::Modifier bannerColor(TermColor::FG_GREEN);
        const TermColor::Modifier defaultColor(TermColor::FG_DEFAULT);
        const TermColor::Modifier errorColor(TermColor::FG_RED);

        std::cout << bannerColor << "> uci protocol begins" << std::endl;
        std::cout << defaultColor << std::endl;

        std::string command;
        std::cerr << bannerColor << ">> " << defaultColor;


        while (getline(std::cin, command)) {
            auto start = std::chrono::high_resolution_clock::now();

            if (command == "uci") inputUci();
            else if (command == "quit" || command == "stop") break;
            else if (command == "isready") inputIsReady(rt);
            else if (command.rfind("position", 0) == 0) inputPosition(b, command, rt);
            else if (command.rfind("go", 0) == 0) outputBestMove(b, tt, rt, bk, command);
            else if (command == "debug") debug(b);
            else {
                std::cerr << errorColor << "> Unknown command: " << command << defaultColor << std::endl;
            }

            std::cerr << errorColor << "[" << std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - start).count() << "ms] ";
            std::cerr << bannerColor << ">> " << defaultColor;
        }

    } 

    void inputUci() {
        std::cout << "id name " << engineName << std::endl;
        std::cout << "id author " << engineAuthor << std::endl;

        std::cout << "uciok" << std::endl;
    }

    void inputIsReady(RepetitionTable &rt) {
        std::cout << "readyok" << std::endl;
    }

    void inputPosition(Board &b, const std::string &command, RepetitionTable &rt) {
        std::istringstream iss(command);

        std::string position;
        std::string option;
        iss >> position >> option;

        if (option == "startpos") {
            static std::string defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
            b.setFEN(defaultFEN);
            rt.increment(b.polyglotHash);
            initialStack.push(b.polyglotHash);
        }
        else if (option == "fen") {
            std::string FEN; 
            int FENWords = 6;
            std::string FENWord;
            while (FENWords-- && iss >> FENWord) {
                FEN += FENWord + " ";
            }
            b.setFEN(FEN);
        }

        if (iss >> option && option == "moves") {
            std::string move;
            while (iss >> move) {
                b.makeMove(parseMove(b, move));
                rt.increment(b.polyglotHash);
                initialStack.push(b.polyglotHash);
            }
        }
    }

    void outputBestMove(Board &b, TTable &tt, RepetitionTable &rt, openingbook::Book &bk, std::string &command) {
        std::istringstream iss(command);

        std::string go;
        std::string option;

        iss >> go >> option;

        int nodes = 0;
        int score = 0;
        int depth = 0;
        int maxDepth = 60;

        int maxTime = 1000;

        if (option == "infinite") {
            maxTime = 100000;
        }
        else if (option == "depth") {
            iss >> maxDepth;
            maxTime *= 10;
        }
        else if (option == "movetime") {
            iss >> maxTime;
        }
        else if (option == "wtime") {
            if (b.currState.currentPlayer == WHITE) {
                iss >> maxTime;
                maxTime /= 40;
            }
            else {
                iss >> go >> option >> maxTime;
                maxTime /= 40;
            }
        }

        Move bestMove = search::bestMove(b, tt, rt, bk, maxDepth, maxTime, nodes, depth, score);
        std::cout << "info score cp " << score << " depth " << depth << " nodes " << nodes << std::endl;
        std::cout << "bestmove " << bestMove.getUciString() << std::endl;

        while (!initialStack.empty()) {
            rt.decrement(initialStack.top());
            initialStack.pop();
        }
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


        if (fromSquare == E1 && toSquare == G1 && b.board[fromSquare] == Piece{KING, WHITE}) 
            return (Move(E1, G1, MoveFlag::CASTLE_KINGSIDE));
        if (fromSquare == E1 && toSquare == C1 && b.board[fromSquare] == Piece{KING, WHITE}) 
            return (Move(E1, C1, MoveFlag::CASTLE_QUEENSIDE));
        if (fromSquare == E8 && toSquare == G8 && b.board[fromSquare] == Piece{KING, BLACK}) 
            return (Move(E8, G8, MoveFlag::CASTLE_KINGSIDE));
        if (fromSquare == E8 && toSquare == C8 && b.board[fromSquare] == Piece{KING, BLACK}) 
            return (Move(E8, C8, MoveFlag::CASTLE_QUEENSIDE));

        if (fromSquare == E1 && toSquare == H1 && b.board[fromSquare] == Piece{KING, WHITE}) 
            return (Move(E1, G1, MoveFlag::CASTLE_KINGSIDE));
        if (fromSquare == E1 && toSquare == A1 && b.board[fromSquare] == Piece{KING, WHITE})
            return (Move(E1, C1, MoveFlag::CASTLE_QUEENSIDE));
        if (fromSquare == E8 && toSquare == H8 && b.board[fromSquare] == Piece{KING, BLACK}) 
            return (Move(E8, G8, MoveFlag::CASTLE_KINGSIDE));
        if (fromSquare == E8 && toSquare == A8 && b.board[fromSquare] == Piece{KING, BLACK}) 
            return (Move(E8, C8, MoveFlag::CASTLE_QUEENSIDE));


        if (b.board[toSquare].pieceType != N_PIECES) {
            return (Move(fromSquare, toSquare, MoveFlag::CAPTURE));
        }

        if (b.board[fromSquare].pieceType == PAWN) {
            if (b.currState.currentPlayer == WHITE && toSquare == fromSquare - 16) 
                return (Move(fromSquare, toSquare, DOUBLE_PUSH));
            if (b.currState.currentPlayer == BLACK && toSquare == fromSquare + 16) 
                return (Move(fromSquare, toSquare, DOUBLE_PUSH));
        }

        if (toSquare == b.currState.enPassantSquare && b.board[fromSquare].pieceType == PAWN) 
            return (Move(fromSquare, toSquare, EN_PASSANT));
        
        return Move(fromSquare, toSquare);
    }
}
