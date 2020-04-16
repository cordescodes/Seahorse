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
        uint64_t WR = 0ULL; //white rook
        uint64_t WN = 0ULL; //white knight
        uint64_t WB = 0ULL; //white bishop
        uint64_t WQ = 0ULL; //white queen
        uint64_t WK = 0ULL; //white king
        uint64_t BP = 0ULL; //black pawn
        uint64_t BR = 0ULL; //black rook
        uint64_t BN = 0ULL; //black knight
        uint64_t BB = 0ULL; //black bishop
        uint64_t BQ = 0ULL; //black queen
        uint64_t BK = 0ULL; //black king
    private:
        void createBoard();
};

#endif // BOARD_H
