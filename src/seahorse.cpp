#include "moves.h"
#include "board.h"
#include "perft.h"
#include "search.h"
#include <vector>
#include <string>
#include <stdlib.h>

std::string NAME = "Seahorse 1.0";
std::string AUTHOR = "Sam Cordes";

/* Chessmen */
uint64_t WP; //white pawn bit board (64 bit binary converted to unsigned long long)
uint64_t WR; //white rook bit board
uint64_t WN; //white knight bit board
uint64_t WB; //white bishop bit board
uint64_t WQ; //white queen bit board
uint64_t WK; //white king bit board
uint64_t BP; //black pawn bit board
uint64_t BR; //black rook bit board
uint64_t BN; //black knight bit board
uint64_t BB; //black bishop bit board
uint64_t BQ; //black queen bit board
uint64_t BK; //black king bit board

uint64_t EP; //en passant possibility bit board

/* Boolean castling rights */
bool WKC; // white kingside castle
bool WQC; // white queenside castle
bool BKC; // black kingside castle
bool BQC; // black queenside castle

uint64_t EMPTY;

int SEARCH_DEPTH;

Moves moves;
Search searching;

std::vector<Move> history;

bool whiteMove;

void newGame()
{
    Board chessBoard = Board(); // Create new board

    history.clear();

    whiteMove = true;

    SEARCH_DEPTH = 5;

    /* Assign starting location of chessmen based upon setup in board.cpp */
    WP = chessBoard.WP;
    WR = chessBoard.WR;
    WN = chessBoard.WN;
    WB = chessBoard.WB;
    WQ = chessBoard.WQ;
    WK = chessBoard.WK;
    BP = chessBoard.BP;
    BR = chessBoard.BR;
    BN = chessBoard.BN;
    BB = chessBoard.BB;
    BQ = chessBoard.BQ;
    BK = chessBoard.BK;

    EP = 0ULL; // no possibility of en passant to begin game

    /* Each side has castling rights at start of game */
    WKC = true;
    WQC = true;
    BKC = true;
    BQC = true;
}

int getFile(char file)
{
    if (file == 'a') {
        return 7;
    } else if (file == 'b') {
        return 6;
    } else if (file == 'c') {
        return 5;
    } else if (file == 'd') {
        return 4;
    } else if (file == 'e') {
        return 3;
    } else if (file == 'f') {
        return 2;
    } else if (file == 'g') {
        return 1;
    } else if (file == 'h') {
        return 0;
    } else if (file == '0') {
        std::cout << "NULL move: " << file << std::endl;
        return -1;
    } else {
        std::cout << "INVALID file: " << file << std::endl;
        return -1;
    }
}

int whatChessman(uint64_t loc)
{ //1 = pawn, 2 = rook, 3 = knight, 4 = bishop, 5 = queen, 6 = king
    if (((loc&WP)) != 0ULL || ((loc&BP)) != 0ULL) { // Pawn
        return 1;
    } else if (((loc&WR) != 0ULL || ((loc&BR)) != 0ULL)) { // Rook
        return 2;
    } else if (((loc&WN) != 0ULL || ((loc&BN)) != 0ULL)) { // Knight
        return 3;
    } else if (((loc&WB) != 0ULL || ((loc&BB)) != 0ULL)) { // Bishop
        return 4;
    } else if (((loc&WQ) != 0ULL || ((loc&BQ)) != 0ULL)) { // Queen
        return 5;
    } else if (((loc&WK) != 0ULL || ((loc&BK)) != 0ULL)) { // King
        return 6;
    } else {
        std::cout << "INVALID start location: " << loc << std::endl;
        return -1;
    }
}

char numToAlgFile(char File)
{
    if (File == 0) {
        return 'h';
    } else if (File == 1) {
        return 'g';
    } else if (File == 2) {
        return 'f';
    } else if (File == 3) {
        return 'e';
    } else if (File == 4) {
        return 'd';
    } else if (File == 5) {
        return 'c';
    } else if (File == 6) {
        return 'b';
    } else if (File == 7) {
        return 'a';
    } else {
        std::cout << "INVALID numToAlg File: " << File << std::endl;
        return -1;
    }
}

std::string moveToAlg(Move theMove)
{
    std::string algebra = "xxxxx"; // each char will change
    char File, Rank;

    int start = __builtin_ctzll(theMove.start);
    int dest = __builtin_ctzll(theMove.dest);

    File = start % 8;
    Rank = start / 8;

    algebra[0] = numToAlgFile(File);
    algebra[1] = Rank + 49;

    File = dest % 8;
    Rank = dest / 8;

    algebra[2] = numToAlgFile(File);
    algebra[3] = Rank + 49;

    // check for promotion
    if (theMove.special == 3) {
        algebra[4] = 'q';
    } else if (theMove.special == 4) {
        algebra[4] = 'r';
    } else if (theMove.special == 5) {
        algebra[4] = 'b';
    } else if (theMove.special == 6) {
        algebra[4] = 'n';
    } else {
        algebra[4] = ' ';
    }
    return algebra;
}

Move algToMove(std::string algebra) // Make the move, assume move is legal
{ // a = 7, b = 6, c = 5, d = 4, e = 3, f = 2, g = 1, h = 0 ... plus (8*rankNum)
    Move nextMove;
    int loc, startFile, destFile;
    EMPTY = ~(WP|WR|WN|WB|WQ|WK|BP|BR|BN|BB|BQ|BK);
    if (algebra.length() >= 4) {

        nextMove.special = 0;
        // Find starting location
        startFile = getFile(algebra[0]);
        loc = startFile+(8*(algebra[1]-1));
        nextMove.start = 1ULL<<loc;

        // Determine what piece is moving
        nextMove.chessman = whatChessman(nextMove.start);

        // Find destination
        destFile = getFile(algebra[2]);
        loc = destFile+(8*(algebra[3]-1));
        nextMove.dest = 1ULL<<loc;

        // Determine 'special' (0 = normal, 1 = castle, 2 = en passant, 3 = promote queen, 4 = promote rook, 5 = promote bishop, 6 = promote knight)
        if (algebra.length() >= 5) { // Possible promotion (index at 5 could be space)
            if        (algebra[4] == 'q') {
                nextMove.special = 3;
            } else if (algebra[4] == 'r') {
                nextMove.special = 4;
            } else if (algebra[4] == 'b') {
                nextMove.special = 5;
            } else if (algebra[4] == 'n') {
                nextMove.special = 6;
            }
        }

        // Possible en passant
        if (nextMove.chessman == 1 && (startFile != destFile) && ((nextMove.dest&EMPTY)) != 0ULL) { //Pawn move and different file (i.e. capture) and dest is empty
            nextMove.special = 2;
        }

        // Check for castle
        if (nextMove.start == 8ULL && nextMove.dest == 2ULL && WK == 8ULL) { // white kingside castle
            nextMove.special = 1;
        } else if (nextMove.start == 8ULL && nextMove.dest == 32ULL && WK == 8ULL) { // white queenside castle
            nextMove.special = 1;
        } else if (nextMove.start == 576460752303423488ULL && nextMove.dest == 144115188075855872ULL && BK == 576460752303423488ULL) { // black kingside castle
            nextMove.special = 1;
        } else if (nextMove.start == 576460752303423488ULL && nextMove.dest == 2305843009213693952ULL && BK == 576460752303423488ULL) { // black queenside castle
            nextMove.special = 1;
        }

        // Make the move
        WP = moves.makeMove(nextMove, WP, 1);
        WR = moves.makeMove(nextMove, WR, 2);
        WN = moves.makeMove(nextMove, WN, 3);
        WB = moves.makeMove(nextMove, WB, 4);
        WQ = moves.makeMove(nextMove, WQ, 5);
        WK = moves.makeMove(nextMove, WK, 6);
        BP = moves.makeMove(nextMove, BP, 1);
        BR = moves.makeMove(nextMove, BR, 2);
        BN = moves.makeMove(nextMove, BN, 3);
        BB = moves.makeMove(nextMove, BB, 4);
        BQ = moves.makeMove(nextMove, BQ, 5);
        BK = moves.makeMove(nextMove, BK, 6);
        // AFTER the move has been made, check for castling and en passant changes

        // Change castling rights
        if (nextMove.start == 8ULL) { // White king moved
            WKC = false;
            WQC = false;
        } else if (nextMove.start == 576460752303423488ULL) { // Black king moved
            BKC = false;
            BQC = false;
        } else if (nextMove.start == 1ULL || nextMove.dest == 1ULL) { // White kingside rook moved or captured
            WKC = false;
        } else if (nextMove.start == 128ULL || nextMove.dest == 128ULL) { // White queenside rook moved or captured
            WQC = false;
        } else if (nextMove.start == 72057594037927936ULL || nextMove.dest == 72057594037927936ULL) { // Black kingside rook moved or captured
            BKC = false;
        } else if (nextMove.start == 9223372036854775808ULL || nextMove.dest == 9223372036854775808ULL) { // Black queenside rook moved or captured
            BQC = false;
        }

        // En passant possibility
        if ((nextMove.chessman == 1) && (((nextMove.start<<16) == nextMove.dest) || ((nextMove.start>>16) == nextMove.dest))) {
            EP = nextMove.dest;
        } else {
            EP = 0ULL;
        }

    } else {
        std::cout << "INVALID algebra length: " << algebra.length() << " " << algebra << std::endl;
    }
    return nextMove;
}

bool checkLegalMove(Move theMove, bool WhiteMove, uint64_t tWP, uint64_t tWR, uint64_t tWN, uint64_t tWB, uint64_t tWQ, uint64_t tWK, uint64_t tBP, uint64_t tBR, uint64_t tBN, uint64_t tBB, uint64_t tBQ, uint64_t tBK)
{
    tWP = moves.makeMove(theMove, tWP, 1);
    tWR = moves.makeMove(theMove, tWR, 2);
    tWN = moves.makeMove(theMove, tWN, 3);
    tWB = moves.makeMove(theMove, tWB, 4);
    tWQ = moves.makeMove(theMove, tWQ, 5);
    tWK = moves.makeMove(theMove, tWK, 6);
    tBP = moves.makeMove(theMove, tBP, 1);
    tBR = moves.makeMove(theMove, tBR, 2);
    tBN = moves.makeMove(theMove, tBN, 3);
    tBB = moves.makeMove(theMove, tBB, 4);
    tBQ = moves.makeMove(theMove, tBQ, 5);
    tBK = moves.makeMove(theMove, tBK, 6);
    if (WhiteMove) { // White move
        if ((moves.attackedByBlack(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&tWK) != 0ULL) {
            return false;
        }
        if (theMove.special == 1) { // Castle
            if (theMove.dest == 2ULL) { // white kingside castle
                if ((moves.attackedByBlack(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&14ULL) != 0ULL) {
                    return false;
                }
            } else if (theMove.dest == 32ULL) { // white queenside castle
                if ((moves.attackedByBlack(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&56ULL) != 0ULL) {
                   return false;
                }
            }
        }
    } else { // Black move
        if ((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&tBK) != 0ULL) {
            if ((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&tBK) != 0ULL) {
                return false;
            }
            if (theMove.special == 1) { // Castle
                if (theMove.dest == 144115188075855872ULL) { // black kingside castle
                    if ((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&1008806316530991104ULL) != 0ULL) {
                        return false;
                    }
                } else if (theMove.dest == 2305843009213693952ULL) { // black queenside castle
                    if ((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&4035225266123964416ULL) != 0ULL) {
                       return false;
                    }
                }
            }
        }
    }
    return true;
}

void uci()
{
    std::cout << "id name " << NAME << std::endl;
    std::cout << "id author " << AUTHOR << std::endl;
    std::cout << "uciok" << std::endl;
}

void setOption()
{

}

void isReady()
{
    std::cout << "readyok" << std::endl;
}

void newPosition(std::string input)
{
    if (input.find("startpos") != std::string::npos) {
        newGame();
        if (input.find("moves") != std::string::npos) {
            std::string subInput = input.substr(24); // Crop off "position startpos moves"
            while (subInput.length() >= 4) {
                // Check for promotion
                if (subInput.length() >= 5) {
                    if (subInput[4] == 'q'
                     || subInput[4] == 'r'
                     || subInput[4] == 'b'
                     || subInput[4] == 'n') {
                        if (subInput.length() >= 6) { // Promotion is not last move (take space)
                            history.push_back(algToMove(subInput.substr(0,6)));
                            subInput = subInput.substr(6);
                        } else { // Last move is promotion
                            history.push_back(algToMove(subInput.substr(0,5)));
                            subInput = subInput.substr(5);
                        }
                     } else { // Move in the middle which is not promotion (take space)
                        history.push_back(algToMove(subInput.substr(0,5)));
                        subInput = subInput.substr(5);
                    }
                } else { // Last move
                    history.push_back(algToMove(subInput.substr(0,4)));
                    subInput = subInput.substr(4);
                 }
                whiteMove = (!whiteMove); // Change turn
            }
        }
    } else if (input.find("fen") != std::string::npos) {
        std::cout << "ERROR: Seahorse does not support FEN notation." << std::endl;
    } else {
        std::cout << "INVALID position sent: " << input << std::endl;
    }
}

void go()
{
    std::vector<Move> moveList;
    Move bestMove;
    if (whiteMove) { // White's turn
        moveList = moves.possibleMovesWhite(WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK, EP, WKC, WQC);
    } else { // Black's turn
        moveList = moves.possibleMovesBlack(WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK, EP, BKC, BQC);
    }
    bestMove = searching.beginSearch(SEARCH_DEPTH, whiteMove, moveList, WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK, EP, WKC, WQC, BKC, BQC);
    std::cout << "bestmove " << moveToAlg(bestMove) << std::endl;
}

void gameLoop()
{
    std::string input;
    while (1) {
        std::getline(std::cin, input);
        if (input == "uci") {
            uci(); // bootstrap
        } else if (input.find("setoption") != std::string::npos) {
            setOption();
        } else if (input.find("isready") != std::string::npos) {
            isReady();
        } else if (input.find("ucinewgame") != std::string::npos) {
            newGame();
        } else if (input.find("position") != std::string::npos) {
            newPosition(input); // receive new position
        } else if (input.find("go") != std::string::npos) {
            go(); // search for best move
        } else if (input.find("quit") != std::string::npos) {
            break;
        } else if (input.find("print") != std::string::npos) {
            moves.drawBoard(WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK);
        }
    }
}

int main()
{
    newGame();
    moves = Moves(); // Instance of class to find every possible move
    searching = Search(); // Instance of class to find best move

    gameLoop();

    return 0;
}
