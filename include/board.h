#ifndef BOARD_H
#define BOARD_H
#include <cstdint>
#include <iostream>

class Board
{
    public:
        Board();
        ~Board();
        uint64_t WP = 0ULL; //white pawn bit board (64 bit binary converted to unsigned long long)
        uint64_t WR = 0ULL; //white rook bit board
        uint64_t WN = 0ULL; //white knight bit board
        uint64_t WB = 0ULL; //white bishop bit board
        uint64_t WQ = 0ULL; //white queen bit board
        uint64_t WK = 0ULL; //white king bit board
        uint64_t BP = 0ULL; //black pawn bit board
        uint64_t BR = 0ULL; //black rook bit board
        uint64_t BN = 0ULL; //black knight bit board
        uint64_t BB = 0ULL; //black bishop bit board
        uint64_t BQ = 0ULL; //black queen bit board
        uint64_t BK = 0ULL; //black king bit board
    private:
        void createBoard();
};

#endif // BOARD_H
