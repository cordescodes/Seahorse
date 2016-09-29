#ifndef SEARCH_H
#define SEARCH_H
#include "moves.h"
#include "hashmap.h"
#include <climits>
#include <cmath>
#include <algorithm>
#include <random>

struct sortPriority {
    bool operator() (Move move1, Move move2) { return (move1.priority>move2.priority); }
};

class Search
{
    public:
        Search();
        ~Search();

        Move beginSearch(int depth, bool whiteToMove, std::vector<Move> moveList, uint64_t WP, uint64_t WR, uint64_t WN, uint64_t WB, uint64_t WQ, uint64_t WK, uint64_t BP, uint64_t BR, uint64_t BN, uint64_t BB, uint64_t BQ, uint64_t BK, uint64_t EP, bool WKC, bool WQC, bool BKC, bool BQC);
    private:
        float alphaBeta(float alpha, float beta, int depth, bool whiteToMove, uint64_t WP, uint64_t WR, uint64_t WN, uint64_t WB, uint64_t WQ, uint64_t WK, uint64_t BP, uint64_t BR, uint64_t BN, uint64_t BB, uint64_t BQ, uint64_t BK, uint64_t EP, bool WKC, bool WQC, bool BKC, bool BQC);
        float scoreBoard(bool whiteToMove, int depth, uint64_t WP, uint64_t WR, uint64_t WN, uint64_t WB, uint64_t WQ, uint64_t WK, uint64_t BP, uint64_t BR, uint64_t BN, uint64_t BB, uint64_t BQ, uint64_t BK, uint64_t EP, bool WKC, bool WQC, bool BKC, bool BQC);

        Moves moves;
        int MAX_DEPTH;
        HashMap hashMap;

        std::string moveToAlg(Move theMove);
        char numToAlgFile(char File);
        int counter; // FOR TESTING
};

#endif // SEARCH_H
