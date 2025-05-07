#include "../include/board.hpp"
#include "../include/movegen.hpp"
#include "../include/zobrist.hpp"

#include <cctype>
#include <sstream>
#include <string>
#include <map>

Board::Board(std::string FEN) {
    std::stringstream split1(FEN);
    std::string boardFEN;
    split1 >> boardFEN;

    std::stringstream rowsplit(boardFEN);
    std::string currRow;
    size_t rank = 0;

    for (size_t i = 0; i < 8; i++) 
        for (size_t j = 0; j < 8; j++) 
            this->board[i * 8 + j] = Piece{N_PIECES, N_COLORS};


    while (!rowsplit.eof()) {
        std::getline(rowsplit, currRow, '/');
        size_t l = currRow.length();
        size_t file = 0;

        for (size_t i = 0; i < l; i++) {
            Square square = static_cast<Square>(rank * 8 + file);
            if (std::isalpha(currRow[i])) {
                size_t col = 0;

                if (std::islower(currRow[i])) {
                    col = 1;
                    currRow[i] -= 32;
                }

                size_t pie = pieceNumberMap.at(currRow[i]);
                bitboard::setbit(bitboards[col][pie], square);
                bitboard::setbit(bitboards[col][6], square);
                this->board[rank * 8 + file] = Piece{static_cast<PieceType>(pie), static_cast<Color>(col)};

                file++;
            }
            else {
                size_t dig = static_cast<size_t>(currRow[i] - '0');
                file += dig;
            }
        }
        rank++;
    }


    std::string activeColor, castlingRights, enPassantSquareFEN;
    split1 >> activeColor >> castlingRights >> enPassantSquareFEN;

    if (activeColor == "b") currState.currentPlayer = BLACK;
    else currState.currentPlayer = WHITE;

    const std::map<char, uint8_t> maptocastle = {
        { 'Q', CASTLE_QUEEN_WHITE },
        { 'K', CASTLE_KING_WHITE},
        { 'q', CASTLE_QUEEN_BLACK},
        { 'k', CASTLE_KING_BLACK},
    };

    currState.castlingState = 0;
    if (castlingRights != "-") {
        for (auto c : castlingRights)
            currState.castlingState |= maptocastle.at(c);
    }

    if (enPassantSquareFEN == "-") currState.enPassantSquare = N_SQUARES;
    else currState.enPassantSquare = wordSquare.at(enPassantSquareFEN);

    this->captured.push({N_PIECES, N_COLORS});

    const Color opps = static_cast<Color>(static_cast<int>(currState.currentPlayer ^ 1));

    currState.isInCheck = moveGen::isSquareAttacked(bitboards, 
            bitboard::getLsb(bitboards[currState.currentPlayer][KING]), opps);

    this->polyglotHash = zobrist::hashBoard(*this);
}

void Board::makeMove(Move mv) {
    const Piece fromPiece = board[mv.from()];

    if (currState.enPassantSquare != N_SQUARES) {
        if (currState.polyglotEnPassant) {
            polyglotHash ^= zobrist::getEnPassantHash(currState.enPassantSquare);
        }
    }

    bitboard::toggleBit(bitboards[fromPiece.color][fromPiece.pieceType], mv.from());
    bitboard::toggleBit(bitboards[fromPiece.color][fromPiece.pieceType], mv.to());

    bitboard::toggleBit(bitboards[fromPiece.color][6], mv.from());
    bitboard::toggleBit(bitboards[fromPiece.color][6], mv.to());

    polyglotHash ^= zobrist::getPieceHash(fromPiece, mv.from());
    polyglotHash ^= zobrist::getPieceHash(fromPiece, mv.to());

    Square captureSquare = mv.to();

    if (mv.isEnPassant()) {
        if (currState.currentPlayer == WHITE) captureSquare = static_cast<Square>(captureSquare + 8);
        else captureSquare = static_cast<Square>(captureSquare - 8);
    }

    const Piece capturePiece = board[captureSquare];

    board[mv.from()] = Piece{N_PIECES, N_COLORS};
    board[mv.to()] = fromPiece;

    if (mv.isCapture()) {
        bitboard::toggleBit(bitboards[capturePiece.color][capturePiece.pieceType], captureSquare);
        bitboard::toggleBit(bitboards[capturePiece.color][6], captureSquare);

        polyglotHash ^= zobrist::getPieceHash(capturePiece, captureSquare);

        if (mv.isEnPassant()) board[captureSquare] = Piece{N_PIECES, N_COLORS};
        this->captured.push(capturePiece);
    }

    if (mv.isPromotion()) {
        const Piece toPromote = Piece{ mv.promotionPiece(), currState.currentPlayer };
        bitboard::toggleBit(bitboards[fromPiece.color][fromPiece.pieceType], mv.to());
        bitboard::toggleBit(bitboards[toPromote.color][toPromote.pieceType], mv.to());

        polyglotHash ^= zobrist::getPieceHash(fromPiece, mv.to());
        polyglotHash ^= zobrist::getPieceHash(toPromote, mv.to());

        board[mv.to()] = toPromote;
    }

    const Square rookFromSquareK = static_cast<Square>(static_cast<int>(mv.from() + 3));
    const Square rookToSquareK = static_cast<Square>(static_cast<int>(mv.to() - 1));
    const Square rookFromSquareQ = static_cast<Square>(static_cast<int>(mv.from() - 4));
    const Square rookToSquareQ = static_cast<Square>(static_cast<int>(mv.to() + 1));

    if (mv.isCastle()) {
        switch (mv.move >> 12) {
            case CASTLE_KINGSIDE:
                bitboard::toggleBit(bitboards[fromPiece.color][ROOK], rookFromSquareK);
                bitboard::toggleBit(bitboards[fromPiece.color][ROOK], rookToSquareK);

                polyglotHash ^= zobrist::getPieceHash(fromPiece, rookFromSquareK);
                polyglotHash ^= zobrist::getPieceHash(fromPiece, rookToSquareK);

                bitboard::toggleBit(bitboards[fromPiece.color][6], rookFromSquareK);
                bitboard::toggleBit(bitboards[fromPiece.color][6], rookToSquareK);

                board[rookFromSquareK] = {N_PIECES, N_COLORS};
                board[rookToSquareK] = {ROOK, fromPiece.color};
                break;

            case CASTLE_QUEENSIDE:
                bitboard::toggleBit(bitboards[fromPiece.color][ROOK], rookFromSquareQ);
                bitboard::toggleBit(bitboards[fromPiece.color][ROOK], rookToSquareQ);

                polyglotHash ^= zobrist::getPieceHash(fromPiece, rookFromSquareQ);
                polyglotHash ^= zobrist::getPieceHash(fromPiece, rookToSquareQ);

                bitboard::toggleBit(bitboards[fromPiece.color][6], rookFromSquareQ);
                bitboard::toggleBit(bitboards[fromPiece.color][6], rookToSquareQ);

                board[rookFromSquareQ] = {N_PIECES, N_COLORS};
                board[rookToSquareQ] = {ROOK, fromPiece.color};
                break;
        }
    }

    gameStates.push(currState);

    polyglotHash ^= zobrist::getCastleHash(currState.castlingState);

    switch (fromPiece.pieceType) {
        case KING:
            switch(fromPiece.color) {
                case WHITE: 
                    currState.castlingState &= ~(CASTLE_KING_WHITE | CASTLE_QUEEN_WHITE); 
                    break;
                case BLACK: 
                    currState.castlingState &= ~(CASTLE_KING_BLACK | CASTLE_QUEEN_BLACK); 
                    break;
                default: break;
            }
            break;
        case ROOK:
            switch (mv.from()) {
                case H1: currState.castlingState &= ~(CASTLE_KING_WHITE); break;
                case A1: currState.castlingState &= ~(CASTLE_QUEEN_WHITE); break;
                case H8: currState.castlingState &= ~(CASTLE_KING_BLACK); break;
                case A8: currState.castlingState &= ~(CASTLE_QUEEN_BLACK); break;
                default: break;
            }
            break;
        default: break;
    }

    if (mv.isCapture()) {
        switch (mv.to()) {
            case H1: currState.castlingState &= ~(CASTLE_KING_WHITE); break;
            case A1: currState.castlingState &= ~(CASTLE_QUEEN_WHITE); break;
            case H8: currState.castlingState &= ~(CASTLE_KING_BLACK); break;
            case A8: currState.castlingState &= ~(CASTLE_QUEEN_BLACK); break;
            default: break;
        }
    }


    polyglotHash ^= zobrist::getCastleHash(currState.castlingState);

    const Color opps = static_cast<Color>(static_cast<int>(currState.currentPlayer) ^ 1);

    if (mv.isDoublePush()) {
        currState.enPassantSquare = static_cast<Square>(static_cast<int>((mv.from() + mv.to()) >> 1));

        if (moveGen::isEnPassantAttacked(bitboards, currState.enPassantSquare, opps)) {
            currState.polyglotEnPassant = true;
            polyglotHash ^= zobrist::getEnPassantHash(currState.enPassantSquare);
        }
    }
    else currState.enPassantSquare = N_SQUARES;

    polyglotHash ^= zobrist::getTurnHash(currState.currentPlayer);

    currState.currentPlayer = opps;

    polyglotHash ^= zobrist::getTurnHash(currState.currentPlayer);

    const Square oppKingSquare = bitboard::getLsb(bitboards[currState.currentPlayer][KING]);
    currState.isInCheck = moveGen::isSquareAttacked(bitboards, oppKingSquare, fromPiece.color);

    moves.push(mv);
}

void Board::unMakeMove() {

    if (moves.empty()) return;
    Move mv = moves.top();
    moves.pop();

    polyglotHash ^= zobrist::getTurnHash(currState.currentPlayer);
    polyglotHash ^= zobrist::getCastleHash(currState.castlingState);

    if (mv.isDoublePush()) {
        Square enPassantSquare = currState.enPassantSquare;
        if (currState.polyglotEnPassant) {
            polyglotHash ^= zobrist::getEnPassantHash(enPassantSquare);
        }
    }

    currState = gameStates.top();
    gameStates.pop();


    polyglotHash ^= zobrist::getTurnHash(currState.currentPlayer);
    polyglotHash ^= zobrist::getCastleHash(currState.castlingState);

    if (currState.enPassantSquare != N_SQUARES) {
        if (currState.polyglotEnPassant) {
            polyglotHash ^= zobrist::getEnPassantHash(currState.enPassantSquare);
        }
    }

    const Piece fromPiece = board[mv.to()];
    
    bitboard::toggleBit(bitboards[fromPiece.color][fromPiece.pieceType], mv.from());
    bitboard::toggleBit(bitboards[fromPiece.color][fromPiece.pieceType], mv.to());

    bitboard::toggleBit(bitboards[fromPiece.color][6], mv.from());
    bitboard::toggleBit(bitboards[fromPiece.color][6], mv.to());


    polyglotHash ^= zobrist::getPieceHash(fromPiece, mv.from());
    polyglotHash ^= zobrist::getPieceHash(fromPiece, mv.to());

    board[mv.from()] = fromPiece;
    board[mv.to()] = {N_PIECES, N_COLORS};

    if (mv.isCapture()) {
        Square captureSquare = mv.isEnPassant() ? currState.enPassantSquare : mv.to();
        if (mv.isEnPassant()) {
            if (currState.currentPlayer == WHITE) captureSquare = static_cast<Square>(captureSquare + 8);
            else captureSquare = static_cast<Square>(captureSquare - 8);
        }
        const Piece capturePiece = captured.top(); captured.pop();

        bitboard::toggleBit(bitboards[capturePiece.color][capturePiece.pieceType], captureSquare);
        bitboard::toggleBit(bitboards[capturePiece.color][6], captureSquare);

        polyglotHash ^= zobrist::getPieceHash(capturePiece, captureSquare);

        board[captureSquare] = capturePiece;
    }
    if (mv.isPromotion()) {
        const Piece toPromote = Piece{ mv.promotionPiece(), currState.currentPlayer };

        bitboard::toggleBit(bitboards[fromPiece.color][fromPiece.pieceType], mv.from());
        bitboard::toggleBit(bitboards[toPromote.color][PAWN], mv.from());


        polyglotHash ^= zobrist::getPieceHash(fromPiece, mv.from());
        polyglotHash ^= zobrist::getPieceHash({PAWN, toPromote.color}, mv.from());

        board[mv.from()] = {PAWN, fromPiece.color};
    }

    const Square rookFromSquareK = static_cast<Square>(static_cast<int>(mv.from() + 3));
    const Square rookToSquareK = static_cast<Square>(static_cast<int>(mv.to() - 1));
    const Square rookFromSquareQ = static_cast<Square>(static_cast<int>(mv.from() - 4));
    const Square rookToSquareQ = static_cast<Square>(static_cast<int>(mv.to() + 1));

    if (mv.isCastle()) {
        switch (mv.move >> 12) {
            case CASTLE_KINGSIDE:
                bitboard::toggleBit(bitboards[fromPiece.color][ROOK], rookFromSquareK);
                bitboard::toggleBit(bitboards[fromPiece.color][ROOK], rookToSquareK);


                polyglotHash ^= zobrist::getPieceHash(fromPiece, rookFromSquareK);
                polyglotHash ^= zobrist::getPieceHash(fromPiece, rookToSquareK);

                bitboard::toggleBit(bitboards[fromPiece.color][6], rookFromSquareK);
                bitboard::toggleBit(bitboards[fromPiece.color][6], rookToSquareK);

                board[rookToSquareK] = {N_PIECES, N_COLORS};
                board[rookFromSquareK] = {ROOK, fromPiece.color};
                break;

            case CASTLE_QUEENSIDE:
                bitboard::toggleBit(bitboards[fromPiece.color][ROOK], rookFromSquareQ);
                bitboard::toggleBit(bitboards[fromPiece.color][ROOK], rookToSquareQ);

                polyglotHash ^= zobrist::getPieceHash(fromPiece, rookFromSquareQ);
                polyglotHash ^= zobrist::getPieceHash(fromPiece, rookToSquareQ);

                bitboard::toggleBit(bitboards[fromPiece.color][6], rookFromSquareQ);
                bitboard::toggleBit(bitboards[fromPiece.color][6], rookToSquareQ);

                board[rookToSquareQ] = {N_PIECES, N_COLORS};
                board[rookFromSquareQ] = {ROOK, fromPiece.color};
                break;
        }
    }
}

void Board::setFEN(std::string &FEN) {
    std::stringstream split1(FEN);
    std::string boardFEN;
    split1 >> boardFEN;

    while (moves.size()) moves.pop();
    while (captured.size()) captured.pop();
    while (gameStates.size()) gameStates.pop();

    std::stringstream rowsplit(boardFEN);
    std::string currRow;
    size_t rank = 0;

    for (size_t i = 0; i < 8; i++) 
        for (size_t j = 0; j < 8; j++) 
            this->board[i * 8 + j] = Piece{N_PIECES, N_COLORS};

    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 7; j++) {
            bitboards[i][j] = 0ull;
        }
    }

    while (!rowsplit.eof()) {
        std::getline(rowsplit, currRow, '/');
        size_t l = currRow.length();
        size_t file = 0;

        for (size_t i = 0; i < l; i++) {
            Square square = static_cast<Square>(rank * 8 + file);
            if (std::isalpha(currRow[i])) {
                size_t col = 0;

                if (std::islower(currRow[i])) {
                    col = 1;
                    currRow[i] -= 32;
                }

                size_t pie = pieceNumberMap.at(currRow[i]);
                bitboard::setbit(bitboards[col][pie], square);
                bitboard::setbit(bitboards[col][6], square);
                this->board[rank * 8u + file] = Piece{static_cast<PieceType>(pie), static_cast<Color>(col)};

                file++;
            }
            else {
                size_t dig = static_cast<size_t>(currRow[i] - '0');
                file += dig;
            }
        }
        rank++;
    }


    std::string activeColor, castlingRights, enPassantSquareFEN;
    split1 >> activeColor >> castlingRights >> enPassantSquareFEN;

    if (activeColor == "b") currState.currentPlayer = BLACK;
    else currState.currentPlayer = WHITE;

    const std::map<char, uint8_t> maptocastle = {
        { 'Q', CASTLE_QUEEN_WHITE },
        { 'K', CASTLE_KING_WHITE},
        { 'q', CASTLE_QUEEN_BLACK},
        { 'k', CASTLE_KING_BLACK},
    };

    currState.castlingState = 0;
    if (castlingRights != "-") {
        for (auto c : castlingRights)
            currState.castlingState |= maptocastle.at(c);
    }

    const Color opps = static_cast<Color>(static_cast<int>(currState.currentPlayer ^ 1));

    currState.polyglotEnPassant = false;

    if (enPassantSquareFEN == "-") currState.enPassantSquare = N_SQUARES;
    else {
        currState.enPassantSquare = wordSquare.at(enPassantSquareFEN);
        if (moveGen::isEnPassantAttacked(bitboards, currState.enPassantSquare, currState.currentPlayer)) {
            currState.polyglotEnPassant = true;
        }
    }

    this->captured.push({N_PIECES, N_COLORS});


    currState.isInCheck = moveGen::isSquareAttacked(bitboards, 
            bitboard::getLsb(bitboards[currState.currentPlayer][KING]), opps);

    this->polyglotHash = zobrist::hashBoard(*this);
}

Board::Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {}
