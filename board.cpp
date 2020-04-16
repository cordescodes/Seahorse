#include "board.h"

Board::Board()
{
    createBoard();
}

Board::~Board()
{
}

void Board::createBoard()
{
    uint64_t binary;
    std::string square;
    std::string chessBoard[8][8] =
    {
        "r", "n", "b", "q", "k", "b", "n", "r",
        "p", "p", "p", "p", "p", "p", "p", "p",
        " ", " ", " ", " ", " ", " ", " ", " ",
        " ", " ", " ", " ", " ", " ", " ", " ",
        " ", " ", " ", " ", " ", " ", " ", " ",
        " ", " ", " ", " ", " ", " ", " ", " ",
        "P", "P", "P", "P", "P", "P", "P", "P",
        "R", "N", "B", "Q", "K", "B", "N", "R"
    };

    for (int i=0; i<64; i++) 
    {
        binary = (1ULL<<i);
        square = chessBoard[7-i/8][7-i%8];

        if (square == "P")
            WP = WP|binary;
        else if (square == "p")
            BP = BP|binary;
        else if (square == "R")
            WR = WR|binary;
        else if (square == "r")
            BR = BR|binary;
        else if (square == "N")
            WN = WN|binary;
        else if (square == "n")
            BN = BN|binary;
        else if (square == "B")
            WB = WB|binary;
        else if (square == "b")
            BB = BB|binary;
        else if (square == "Q")
            WQ = WQ|binary;
        else if (square == "q")
            BQ = BQ|binary;
        else if (square == "K")
            WK = WK|binary;
        else if (square == "k")
            BK = BK|binary;
    }
}
