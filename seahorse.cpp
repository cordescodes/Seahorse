#include "moves.h"
#include "board.h"
#include "search.h"
#include <vector>
#include <string>
#include <stdlib.h>

std::string NAME = "Seahorse 1.0";
std::string AUTHOR = "Sam Cordes";

/* Chessmen */
uint64_t WP; //white pawn bit board (64 bit binary converted to unsigned long long)
uint64_t WR; //white rook
uint64_t WN; //white knight
uint64_t WB; //white bishop
uint64_t WQ; //white queen
uint64_t WK; //white king
uint64_t BP; //black pawn
uint64_t BR; //black rook
uint64_t BN; //black knight
uint64_t BB; //black bishop
uint64_t BQ; //black queen
uint64_t BK; //black king

uint64_t EP; //en passant possibility

/* Boolean castling rights */
bool WKC; // white kingside
bool WQC; // white queenside
bool BKC; // black kingside
bool BQC; // black queenside

uint64_t EMPTY;

int SEARCH_DEPTH;

Moves moves;
Search searching;

std::vector<Move> history;

bool whiteMove;

void newGame()
{
    Board chessBoard = Board();

    history.clear();

    whiteMove = true;

    SEARCH_DEPTH = 5;

    // Assign starting piece locations
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

    // each side has castling rights at start of game
    WKC = true;
    WQC = true;
    BKC = true;
    BQC = true;
}

int getFile(char file)
{
    switch (file)
    {
    case 'a':
        return 7;
    case 'b':
        return 6;
    case 'c':
        return 5;
    case 'd':
        return 4;
    case 'e':
        return 3;
    case 'f':
        return 2;
    case 'g':
        return 1;
    case 'h':
        return 0;
    default:
        std::cout << "NULL move: " << file << std::endl;
        return -1;
    }
}

/**
 * Determine which chessman type occupies the specified location in chessboard
 * 1 = pawn, 2 = rook, 3 = knight, 4 = bishop, 5 = queen, 6 = king
 */
int whichChessman(uint64_t loc)
{ 
    if (((loc&WP)) != 0ULL || ((loc&BP)) != 0ULL) // Pawn
        return 1;
    else if (((loc&WR) != 0ULL || ((loc&BR)) != 0ULL)) // Rook
        return 2;
    else if (((loc&WN) != 0ULL || ((loc&BN)) != 0ULL)) // Knight
        return 3;
    else if (((loc&WB) != 0ULL || ((loc&BB)) != 0ULL)) // Bishop
        return 4;
    else if (((loc&WQ) != 0ULL || ((loc&BQ)) != 0ULL)) // Queen
        return 5;
    else if (((loc&WK) != 0ULL || ((loc&BK)) != 0ULL)) // King
        return 6;
    else 
    {
        std::cout << "INVALID start location: " << loc << std::endl;
        return -1;
    }
}

char numToAlgFile(char file)
{
    switch (file)
    {
    case 0:
        return 'h';
    case 1:
        return 'g';
    case 2:
        return 'f';
    case 3:
        return 'e';
    case 4:
        return 'd';
    case 5:
        return 'c';
    case 6:
        return 'b';
    case 7:
        return 'a';
    default:
        std::cout << "INVALID numToAlg File: " << file << std::endl;
        return -1;
    }
}

std::string moveToAlg(Move move)
{
    std::string algebra = "xxxxx"; // each char will change
    char file, rank;

    int start = __builtin_ctzll(move.start);
    int dest = __builtin_ctzll(move.dest);

    file = start % 8;
    rank = start / 8;

    algebra[0] = numToAlgFile(file);
    algebra[1] = rank + 49;

    file = dest % 8;
    rank = dest / 8;

    algebra[2] = numToAlgFile(file);
    algebra[3] = rank + 49;

    // check for promotion
    switch (move.special)
    {
    case 3:
        algebra[4] = 'q';
        break;
    case 4:
        algebra[4] = 'r';
        break;
    case 5:
        algebra[4] = 'b';
        break;
    case 6:
        algebra[4] = 'n';
        break;
    default:
        algebra[4] = ' ';
        break;
    }

    return algebra;
}

Move algToMove(std::string algebra) // Make the move, assume move is legal
{ // a = 7, b = 6, c = 5, d = 4, e = 3, f = 2, g = 1, h = 0 ... plus (8*rankNum)
    Move move;
    int loc, startFile, destFile;
    EMPTY = ~(WP|WR|WN|WB|WQ|WK|BP|BR|BN|BB|BQ|BK);

    if (algebra.length() >= 4) 
    {
        move.special = 0;
        // Find starting location
        startFile = getFile(algebra[0]);
        loc = startFile + (8*(algebra[1]-1));
        move.start = 1ULL<<loc;

        // Determine which piece is moving
        move.chessman = whichChessman(move.start);

        // Find destination
        destFile = getFile(algebra[2]);
        loc = destFile+(8*(algebra[3]-1));
        move.dest = 1ULL<<loc;

        // Determine 'special' (0 = normal, 1 = castle, 2 = en passant, 3 = promote queen, 4 = promote rook, 5 = promote bishop, 6 = promote knight)
        if (algebra.length() >= 5) 
        { 
            // Possible promotion (index at 5 could be space)
            if (algebra[4] == 'q')
                move.special = 3;
            else if (algebra[4] == 'r')
                move.special = 4;
            else if (algebra[4] == 'b')
                move.special = 5;
            else if (algebra[4] == 'n')
                move.special = 6;
        }

        // Check for en passant (pawn moved to unoccupied square in different file)
        if (move.chessman == 1 && (startFile != destFile) && ((move.dest&EMPTY)) != 0ULL)
            move.special = 2;

        // Check for castle
        if (move.start == 8ULL && move.dest == 2ULL && WK == 8ULL) // white kingside castle
            move.special = 1;
        else if (move.start == 8ULL && move.dest == 32ULL && WK == 8ULL) // white queenside castle
            move.special = 1;
        else if (move.start == 576460752303423488ULL && move.dest == 144115188075855872ULL && BK == 576460752303423488ULL) // black kingside castle
            move.special = 1;
        else if (move.start == 576460752303423488ULL && move.dest == 2305843009213693952ULL && BK == 576460752303423488ULL) // black queenside castle
            move.special = 1;
        
        // Make the move
        WP = moves.makeMove(move, WP, 1);
        WR = moves.makeMove(move, WR, 2);
        WN = moves.makeMove(move, WN, 3);
        WB = moves.makeMove(move, WB, 4);
        WQ = moves.makeMove(move, WQ, 5);
        WK = moves.makeMove(move, WK, 6);
        BP = moves.makeMove(move, BP, 1);
        BR = moves.makeMove(move, BR, 2);
        BN = moves.makeMove(move, BN, 3);
        BB = moves.makeMove(move, BB, 4);
        BQ = moves.makeMove(move, BQ, 5);
        BK = moves.makeMove(move, BK, 6);

        // AFTER the move has been made, check for potential castling or en passant changes

        // Check castling rights
        if (move.start == 8ULL) 
        { 
            // White king moved
            WKC = false;
            WQC = false;
        } 
        else if (move.start == 576460752303423488ULL) 
        { 
            // Black king moved
            BKC = false;
            BQC = false;
        } 
        else if (move.start == 1ULL || move.dest == 1ULL) 
        { 
            // White kingside rook moved or captured
            WKC = false;
        } 
        else if (move.start == 128ULL || move.dest == 128ULL) 
        { 
            // White queenside rook moved or captured
            WQC = false;
        } 
        else if (move.start == 72057594037927936ULL || move.dest == 72057594037927936ULL) 
        { 
            // Black kingside rook moved or captured
            BKC = false;
        } 
        else if (move.start == 9223372036854775808ULL || move.dest == 9223372036854775808ULL) 
        { 
            // Black queenside rook moved or captured
            BQC = false;
        }

        // Check en passant possibility
        if ((move.chessman == 1) && (((move.start<<16) == move.dest) || ((move.start>>16) == move.dest)))
            EP = move.dest;
        else
            EP = 0ULL;

    } 
    else 
        std::cout << "INVALID algebra length: " << algebra.length() << " " << algebra << std::endl;

    return move;
}

/**
 * return true if this is a legal move
 */
bool checkLegalMove(Move move, bool whiteMove, 
    uint64_t tWP, uint64_t tWR, uint64_t tWN, uint64_t tWB, uint64_t tWQ, uint64_t tWK, 
    uint64_t tBP, uint64_t tBR, uint64_t tBN, uint64_t tBB, uint64_t tBQ, uint64_t tBK)
{
    tWP = moves.makeMove(move, tWP, 1);
    tWR = moves.makeMove(move, tWR, 2);
    tWN = moves.makeMove(move, tWN, 3);
    tWB = moves.makeMove(move, tWB, 4);
    tWQ = moves.makeMove(move, tWQ, 5);
    tWK = moves.makeMove(move, tWK, 6);
    tBP = moves.makeMove(move, tBP, 1);
    tBR = moves.makeMove(move, tBR, 2);
    tBN = moves.makeMove(move, tBN, 3);
    tBB = moves.makeMove(move, tBB, 4);
    tBQ = moves.makeMove(move, tBQ, 5);
    tBK = moves.makeMove(move, tBK, 6);

    if (whiteMove) 
    {
        if ((moves.attackedByBlack(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&tWK) != 0ULL)
            return false;
        
        if (move.special == 1) 
        { 
            // Looks like we're castling, did we castle into check?
            if (move.dest == 2ULL) 
            { 
                // white kingside castle
                if ((moves.attackedByBlack(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&14ULL) != 0ULL)
                    return false;
    
            } 
            else if (move.dest == 32ULL) 
            { 
                // white queenside castle
                if ((moves.attackedByBlack(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&56ULL) != 0ULL)
                   return false;
            }
        }
    } 
    else 
    { 
        // Black move
        if ((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&tBK) != 0ULL) 
        {
            if ((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&tBK) != 0ULL)
                return false;
            
            if (move.special == 1) 
            { 
                // Looks like we're castling, did we castle into check?
                if (move.dest == 144115188075855872ULL) 
                { 
                    // black kingside castle
                    if ((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&1008806316530991104ULL) != 0ULL)
                        return false;
                } 
                else if (move.dest == 2305843009213693952ULL) 
                {
                    // black queenside castle
                    if ((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&4035225266123964416ULL) != 0ULL)
                       return false;
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
    // no op
}

void isReady()
{
    std::cout << "readyok" << std::endl;
}

void newPosition(std::string input)
{
    if (input.find("startpos") != std::string::npos) 
    {
        newGame();

        if (input.find("moves") != std::string::npos) 
        {
            std::string subInput = input.substr(24); // Crop off "position startpos moves"
            while (subInput.length() >= 4) 
            {
                // Check for promotion
                if (subInput.length() >= 5) 
                {
                    if (subInput[4] == 'q' || subInput[4] == 'r' || subInput[4] == 'b' || subInput[4] == 'n') 
                     {
                        if (subInput.length() >= 6) 
                        { 
                            // Promotion is not last move (take space)
                            history.push_back(algToMove(subInput.substr(0,6)));
                            subInput = subInput.substr(6);
                        } 
                        else 
                        { 
                            // Last move is promotion
                            history.push_back(algToMove(subInput.substr(0,5)));
                            subInput = subInput.substr(5);
                        }
                     } 
                     else 
                     { 
                        // Move in the middle which is not promotion (take space)
                        history.push_back(algToMove(subInput.substr(0,5)));
                        subInput = subInput.substr(5);
                    }
                } else 
                { 
                    // Last move
                    history.push_back(algToMove(subInput.substr(0,4)));
                    subInput = subInput.substr(4);
                 }
                whiteMove = !whiteMove; // Change turn
            }
        }
        else if (input.find("fen") != std::string::npos)
            std::cout << "ERROR: Seahorse does not support FEN notation." << std::endl;
        else
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
    while (1)
    {
        std::getline(std::cin, input);
        if (input == "uci")
            uci();
        else if (input.find("setoption") != std::string::npos)
            setOption();
        else if (input.find("isready") != std::string::npos)
            isReady();
        else if (input.find("ucinewgame") != std::string::npos)
            newGame();
        else if (input.find("position") != std::string::npos)
            newPosition(input);
        else if (input.find("go") != std::string::npos)
            go();
        else if (input.find("quit") != std::string::npos)
            break;
        else if (input.find("print") != std::string::npos)
            moves.drawBoard(WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK);
    }
}

int main()
{
    newGame();
    moves = Moves();
    searching = Search();

    gameLoop();

    return 0;
}
