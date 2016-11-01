#include "search.h"

Search::Search()
{
    moves = Moves();
}

Search::~Search() // Destructor
{
}

float Search::scoreBoard(bool whiteToMove, uint64_t WP, uint64_t WR, uint64_t WN, uint64_t WB, uint64_t WQ, uint64_t WK, uint64_t BP, uint64_t BR, uint64_t BN, uint64_t BB, uint64_t BQ, uint64_t BK, uint64_t EP, bool WKC, bool WQC, bool BKC, bool BQC)
{
    std::vector<Move> moveList;

    if (whiteToMove) {
        moveList = moves.possibleMovesWhite(WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK, EP, WKC, WQC);
        if (moveList.size() == 0) { // No legal moves (checkmate or stalemate)
            return -9999;
        }
    } else if (!whiteToMove) {
        moveList = moves.possibleMovesBlack(WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK, EP, BKC, BQC);
        if (moveList.size() == 0) { // No legal moves (checkmate or stalemate)
            return 9999;
        }
    }

    return ( __builtin_popcountll(WP) + (3 * __builtin_popcountll(WN)) + (3 * __builtin_popcountll(WB)) + (5 * __builtin_popcountll(WR)) + (9 * __builtin_popcountll(WQ)) )
              - ( __builtin_popcountll(BP) + (3 * __builtin_popcountll(BN)) + (3 * __builtin_popcountll(BB)) + (5 * __builtin_popcountll(BR)) + (9 * __builtin_popcountll(BQ)) );
}

// Recursive algorithm which finds best move
float Search::alphaBeta(float alpha, float beta, int depth, bool whiteToMove, uint64_t WP, uint64_t WR, uint64_t WN, uint64_t WB, uint64_t WQ, uint64_t WK, uint64_t BP, uint64_t BR, uint64_t BN, uint64_t BB, uint64_t BQ, uint64_t BK, uint64_t EP, bool WKC, bool WQC, bool BKC, bool BQC)
{
    float bestScore;
    std::vector<Move> moveList;

    if (depth == MAX_DEPTH) {
        counter++;
        return scoreBoard(!whiteToMove, WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK, EP, WKC, WQC, BKC, BQC);
    }

    if (whiteToMove) {
        moveList = moves.possibleMovesWhite(WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK, EP, WKC, WQC);
        bestScore = alpha;
        if (moveList.size() == 0) { // No legal moves (checkmate or stalemate)
            return -9999;
        }
    } else {
        moveList = moves.possibleMovesBlack(WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK, EP, BKC, BQC);
        bestScore = beta;
        if (moveList.size() == 0) { // No legal moves (checkmate or stalemate)
            return 9999;
        }
    }

    std::sort(moveList.begin(), moveList.end(), sortPriority());

    for (int i = 0; i < moveList.size(); i++) {
        uint64_t tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK, tEP;
        bool tWKC = WKC, tWQC = WQC, tBKC = BKC, tBQC = BQC;
        bool illegalCastle = false;

        // Make the move
        tWP = moves.makeMove(moveList.at(i), WP, 1);
        tWR = moves.makeMove(moveList.at(i), WR, 2);
        tWN = moves.makeMove(moveList.at(i), WN, 3);
        tWB = moves.makeMove(moveList.at(i), WB, 4);
        tWQ = moves.makeMove(moveList.at(i), WQ, 5);
        tWK = moves.makeMove(moveList.at(i), WK, 6);
        tBP = moves.makeMove(moveList.at(i), BP, 1);
        tBR = moves.makeMove(moveList.at(i), BR, 2);
        tBN = moves.makeMove(moveList.at(i), BN, 3);
        tBB = moves.makeMove(moveList.at(i), BB, 4);
        tBQ = moves.makeMove(moveList.at(i), BQ, 5);
        tBK = moves.makeMove(moveList.at(i), BK, 6);
        tEP = 0ULL;

        /* Determine castling rights */
        if ((moveList.at(i).start&8574853690513424246ULL == 0ULL) || (moveList.at(i).dest&8574853690513424246ULL == 0ULL)) { // If move doesn't start or end at king or rook position
            if (moveList.at(i).special == 1) { // Castle
                if (whiteToMove && moveList.at(i).dest == 2ULL) { // white kingside castle
                    if ((moves.attackedByBlack(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&14ULL) != 0ULL) {
                        illegalCastle = true;
                    } else {
                        tWKC = false;
                        tWQC = false;
                    }
                } else if (whiteToMove && moveList.at(i).dest == 32ULL) { // white queenside castle
                    if ((moves.attackedByBlack(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&56ULL) != 0ULL) {
                        illegalCastle = true;
                    } else {
                        tWKC = false;
                        tWQC = false;
                    }
                } else if ((!whiteToMove) && moveList.at(i).dest == 144115188075855872ULL) { // black kingside castle
                    if ((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&1008806316530991104ULL) != 0ULL) {
                        illegalCastle = true;
                    } else {
                        tBKC = false;
                        tBQC = false;
                    }
                } else if ((!whiteToMove) && moveList.at(i).dest == 2305843009213693952ULL) { // black queenside castle
                    if ((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&4035225266123964416ULL) != 0ULL) {
                        illegalCastle = true;
                    } else {
                        tBKC = false;
                        tBQC = false;
                    }
                }
            } else if (moveList.at(i).start == 8ULL) { // White king moved
                tWKC = false;
                tWQC = false;
            } else if (moveList.at(i).start == 576460752303423488ULL) { // Black king moved
                tBKC = false;
                tBQC = false;
            } else if (moveList.at(i).start == 1ULL || moveList.at(i).dest == 1ULL) { // White kingside rook moved or captured
                tWKC = false;
            } else if (moveList.at(i).start == 128ULL || moveList.at(i).dest == 128ULL) { // White queenside rook moved or captured
                tWQC = false;
            } else if (moveList.at(i).start == 72057594037927936ULL || moveList.at(i).dest == 72057594037927936ULL) { // Black kingside rook moved or captured
                tBKC = false;
            } else if (moveList.at(i).start == 9223372036854775808ULL || moveList.at(i).dest == 9223372036854775808ULL) { // Black queenside rook moved or captured
                tBQC = false;
            }
        } else if ((moveList.at(i).chessman == 1)
        && (((moveList.at(i).start<<16) == moveList.at(i).dest) || ((moveList.at(i).start>>16) == moveList.at(i).dest))) { // En passant possibility
            tEP = moveList.at(i).dest;
        }

        if (whiteToMove) {
            // Check if legal move
            if (((moves.attackedByBlack(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&tWK) == 0ULL) && (!illegalCastle)){

                float nextMoveScore = alphaBeta(bestScore, beta, depth+1, !whiteToMove, tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK, tEP, tWKC, tWQC, tBKC, tBQC);
                bestScore = std::max(bestScore, nextMoveScore);
                if (beta <= bestScore) { // Move impossibly good (i.e. opponent blunder)
                    return bestScore;
                }
            }
        } else {
            // Check if legal move
            if (((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&tBK) == 0ULL) && (!illegalCastle)){

                float nextMoveScore = alphaBeta(alpha, bestScore, depth+1, !whiteToMove, tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK, tEP, tWKC, tWQC, tBKC, tBQC);
                bestScore = std::min(bestScore, nextMoveScore);
                if (bestScore <= alpha) { // Move impossibly good (i.e. opponent blunder)
                    return bestScore;
                }
            }
        }
    }

    return bestScore;
}

Move Search::beginSearch(int depth, bool whiteToMove, std::vector<Move> moveList, uint64_t WP, uint64_t WR, uint64_t WN, uint64_t WB, uint64_t WQ, uint64_t WK, uint64_t BP, uint64_t BR, uint64_t BN, uint64_t BB, uint64_t BQ, uint64_t BK, uint64_t EP, bool WKC, bool WQC, bool BKC, bool BQC)
{

    std::vector<float> scores(moveList.size());
    std::vector<Move> choices;
    float target;

    MAX_DEPTH = depth;

    // Increase search depth if fewer pieces on board
    if (__builtin_popcountll(WP|WR|WN|WB|WQ|WK|BP|BR|BN|BB|BQ|BQ) >= 5 && __builtin_popcountll(WP|WR|WN|WB|WQ|WK|BP|BR|BN|BB|BQ|BQ) < 10) {
        MAX_DEPTH += 2;
    } else if (__builtin_popcountll(WP|WR|WN|WB|WQ|WK|BP|BR|BN|BB|BQ|BQ) < 5) {
        MAX_DEPTH += 4;
    }

    counter = 0; // FOR TESTING

    // Score each possible move (checking legality first)
    for (int i = 0; i < moveList.size(); i++) {
        uint64_t tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK, tEP;
        bool tWKC = WKC, tWQC = WQC, tBKC = BKC, tBQC = BQC;
        bool illegalCastle = false;

        // Make the move
        tWP = moves.makeMove(moveList.at(i), WP, 1);
        tWR = moves.makeMove(moveList.at(i), WR, 2);
        tWN = moves.makeMove(moveList.at(i), WN, 3);
        tWB = moves.makeMove(moveList.at(i), WB, 4);
        tWQ = moves.makeMove(moveList.at(i), WQ, 5);
        tWK = moves.makeMove(moveList.at(i), WK, 6);
        tBP = moves.makeMove(moveList.at(i), BP, 1);
        tBR = moves.makeMove(moveList.at(i), BR, 2);
        tBN = moves.makeMove(moveList.at(i), BN, 3);
        tBB = moves.makeMove(moveList.at(i), BB, 4);
        tBQ = moves.makeMove(moveList.at(i), BQ, 5);
        tBK = moves.makeMove(moveList.at(i), BK, 6);
        tEP = 0ULL;

        /* Determine castling rights */
        if ((moveList.at(i).start&8574853690513424246ULL == 0ULL) || (moveList.at(i).dest&8574853690513424246ULL == 0ULL)) { // If move doesn't start or end at king or rook position
            if (moveList.at(i).special == 1) { // Castle
                if (whiteToMove && moveList.at(i).dest == 2ULL) { // white kingside castle
                    if ((moves.attackedByBlack(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&14ULL) != 0ULL) {
                        illegalCastle = true;
                    } else {
                        tWKC = false;
                        tWQC = false;
                    }
                } else if (whiteToMove && moveList.at(i).dest == 32ULL) { // white queenside castle
                    if ((moves.attackedByBlack(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&56ULL) != 0ULL) {
                        illegalCastle = true;
                    } else {
                        tWKC = false;
                        tWQC = false;
                    }
                } else if ((!whiteToMove) && moveList.at(i).dest == 144115188075855872ULL) { // black kingside castle
                    if ((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&1008806316530991104ULL) != 0ULL) {
                        illegalCastle = true;
                    } else {
                        tBKC = false;
                        tBQC = false;
                    }
                } else if ((!whiteToMove) && moveList.at(i).dest == 2305843009213693952ULL) { // black queenside castle
                    if ((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&4035225266123964416ULL) != 0ULL) {
                        illegalCastle = true;
                    } else {
                        tBKC = false;
                        tBQC = false;
                    }
                }
            } else if (moveList.at(i).start == 8ULL) { // White king moved
                tWKC = false;
                tWQC = false;
            } else if (moveList.at(i).start == 576460752303423488ULL) { // Black king moved
                tBKC = false;
                tBQC = false;
            } else if (moveList.at(i).start == 1ULL || moveList.at(i).dest == 1ULL) { // White kingside rook moved or captured
                tWKC = false;
            } else if (moveList.at(i).start == 128ULL || moveList.at(i).dest == 128ULL) { // White queenside rook moved or captured
                tWQC = false;
            } else if (moveList.at(i).start == 72057594037927936ULL || moveList.at(i).dest == 72057594037927936ULL) { // Black kingside rook moved or captured
                tBKC = false;
            } else if (moveList.at(i).start == 9223372036854775808ULL || moveList.at(i).dest == 9223372036854775808ULL) { // Black queenside rook moved or captured
                tBQC = false;
            }
        } else if ((moveList.at(i).chessman == 1)
        && (((moveList.at(i).start<<16) == moveList.at(i).dest) || ((moveList.at(i).start>>16) == moveList.at(i).dest))) { // En passant possibility
            tEP = moveList.at(i).dest;
        }

        // Check if legal move
        if (whiteToMove) {
            if (((moves.attackedByBlack(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&tWK) == 0ULL) && (!illegalCastle)) {
                scores.at(i) = alphaBeta(-999, 999, 1, false, tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK, tEP, tWKC, tWQC, tBKC, tBQC); // keep track of score for each move
            } else { // illegal move
                scores.at(i) = -9999;
            }
        } else {
            if (((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&tBK) == 0ULL) && (!illegalCastle)) {
                scores.at(i) = alphaBeta(-999, 999, 1, true, tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK, tEP, tWKC, tWQC, tBKC, tBQC); // keep track of score for each move
            } else { // illegal move
                scores.at(i) = 9999;
            }
        }
    }

    for (int i = 0; i < scores.size(); i++) // Print out each move and its score
    {
        std::cout << moveToAlg(moveList.at(i)) << " " << scores.at(i) << std::endl;
    }
    std::cout << "Nodes evaluated: " << counter << std::endl; // FOR TESTING

    // Choose best move score depending upon whose turn it is
    if (whiteToMove) {
        target = *std::max_element(scores.begin(), scores.end());
    } else {
        target = *std::min_element(scores.begin(), scores.end());
    }

    // Add all moves which have the best score
    for (int i = 0; i < scores.size(); i++) {
        if (scores.at(i) == target) {
            choices.push_back(moveList.at(i));
        }
    }

    delete &target; // delete iterator (pointer)

    // Return best move (if multiple moves share the same score, choose one at random)
    int index = rand() % choices.size();
    return (choices.at(index));
}

std::string Search::moveToAlg(Move theMove)
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

char Search::numToAlgFile(char File)
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

