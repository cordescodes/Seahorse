#include "search.h"

/**
 * Find best move
 */
Search::Search()
{
    moves = Moves();
}

Search::~Search()
{
}

float Search::scoreBoard(bool whiteToMove, 
    uint64_t WP, uint64_t WR, uint64_t WN, uint64_t WB, uint64_t WQ, uint64_t WK, 
    uint64_t BP, uint64_t BR, uint64_t BN, uint64_t BB, uint64_t BQ, uint64_t BK,
    uint64_t EP, bool WKC, bool WQC, bool BKC, bool BQC)
{
    std::vector<Move> moveList;

    if (whiteToMove) 
    {
        moveList = moves.possibleMovesWhite(WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK, EP, WKC, WQC);
        if (moveList.size() == 0) 
        { 
            // No legal moves (checkmate or stalemate)
            return -9999;
        }
    } 
    else if (!whiteToMove) 
    {
        moveList = moves.possibleMovesBlack(WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK, EP, BKC, BQC);
        if (moveList.size() == 0) 
        { 
            // No legal moves (checkmate or stalemate)
            return 9999;
        }
    }

    return ( __builtin_popcountll(WP) + (3 * __builtin_popcountll(WN)) + (3 * __builtin_popcountll(WB)) + (5 * __builtin_popcountll(WR)) + (9 * __builtin_popcountll(WQ)) )
              - ( __builtin_popcountll(BP) + (3 * __builtin_popcountll(BN)) + (3 * __builtin_popcountll(BB)) + (5 * __builtin_popcountll(BR)) + (9 * __builtin_popcountll(BQ)) );
}

// Recursive algorithm which finds best move
float Search::alphaBeta(float alpha, float beta, int depth, bool whiteToMove, 
    uint64_t WP, uint64_t WR, uint64_t WN, uint64_t WB, uint64_t WQ, uint64_t WK, 
    uint64_t BP, uint64_t BR, uint64_t BN, uint64_t BB, uint64_t BQ, uint64_t BK, 
    uint64_t EP, bool WKC, bool WQC, bool BKC, bool BQC)
{
    float bestScore;
    std::vector<Move> moveList;

    if (depth == MAX_DEPTH) 
    {
        counter++;
        return scoreBoard(!whiteToMove, WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK, EP, WKC, WQC, BKC, BQC);
    }

    if (whiteToMove)
    {
        moveList = moves.possibleMovesWhite(WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK, EP, WKC, WQC);
        bestScore = alpha;
        if (moveList.size() == 0) 
        { 
            // No legal moves (checkmate or stalemate)
            return -9999;
        }
    } else 
    {
        moveList = moves.possibleMovesBlack(WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK, EP, BKC, BQC);
        bestScore = beta;
        if (moveList.size() == 0) 
        { 
            // No legal moves (checkmate or stalemate)
            return 9999;
        }
    }

    std::sort(moveList.begin(), moveList.end(), sortPriority());

    for (int i = 0; i < moveList.size(); i++) 
    {
        uint64_t tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK, tEP;
        bool tWKC = WKC, tWQC = WQC, tBKC = BKC, tBQC = BQC;
        bool illegalCastle = false;

        Move move = moveList.at(i);

        // Make the move
        tWP = moves.makeMove(move, WP, 1);
        tWR = moves.makeMove(move, WR, 2);
        tWN = moves.makeMove(move, WN, 3);
        tWB = moves.makeMove(move, WB, 4);
        tWQ = moves.makeMove(move, WQ, 5);
        tWK = moves.makeMove(move, WK, 6);
        tBP = moves.makeMove(move, BP, 1);
        tBR = moves.makeMove(move, BR, 2);
        tBN = moves.makeMove(move, BN, 3);
        tBB = moves.makeMove(move, BB, 4);
        tBQ = moves.makeMove(move, BQ, 5);
        tBK = moves.makeMove(move, BK, 6);
        tEP = 0ULL;

        // Determine castling rights
        if ((move.start&8574853690513424246ULL == 0ULL) || (move.dest&8574853690513424246ULL == 0ULL)) 
        { 
            // If move doesn't start or end at king or rook position
            if (move.special == 1) 
            { 
                // Castle
                if (whiteToMove && move.dest == 2ULL) 
                { 
                    // white kingside castle
                    if ((moves.attackedByBlack(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&14ULL) != 0ULL)
                        illegalCastle = true;
                    else 
                    {
                        tWKC = false;
                        tWQC = false;
                    }
                } 
                else if (whiteToMove && move.dest == 32ULL) 
                { 
                    // white queenside castle
                    if ((moves.attackedByBlack(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&56ULL) != 0ULL)
                        illegalCastle = true;
                    else 
                    {
                        tWKC = false;
                        tWQC = false;
                    }
                } 
                else if ((!whiteToMove) && move.dest == 144115188075855872ULL) 
                { 
                    // black kingside castle
                    if ((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&1008806316530991104ULL) !=0ULL)
                        illegalCastle = true;
                    else 
                    {
                        tBKC = false;
                        tBQC = false;
                    }
                } 
                else if ((!whiteToMove) && move.dest == 2305843009213693952ULL) 
                { 
                    // black queenside castle
                    if ((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&4035225266123964416ULL) != 0ULL)
                        illegalCastle = true;
                    else 
                    {
                        tBKC = false;
                        tBQC = false;
                    }
                }
            } 
            else if (move.start == 8ULL) 
            { 
                // White king moved
                tWKC = false;
                tWQC = false;
            } 
            else if (move.start == 576460752303423488ULL) 
            { 
                // Black king moved
                tBKC = false;
                tBQC = false;
            } 
            else if (move.start == 1ULL || move.dest == 1ULL) 
            { 
                // White kingside rook moved or captured
                tWKC = false;
            } 
            else if (move.start == 128ULL || move.dest == 128ULL) 
            { 
                // White queenside rook moved or captured
                tWQC = false;
            } 
            else if (move.start == 72057594037927936ULL || move.dest == 72057594037927936ULL) 
            { 
                // Black kingside rook moved or captured
                tBKC = false;
            } 
            else if (move.start == 9223372036854775808ULL || move.dest == 9223372036854775808ULL) 
            { 
                // Black queenside rook moved or captured
                tBQC = false;
            }
        } 
        else if ((move.chessman == 1) && (((move.start<<16) == move.dest) || ((move.start>>16) == move.dest))) 
        { 
            // En passant possibility
            tEP = move.dest;
        }

        if (whiteToMove) 
        {
            // Check if legal move
            if (((moves.attackedByBlack(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&tWK) == 0ULL) && (!illegalCastle))
            {
                float nextMoveScore = alphaBeta(bestScore, beta, depth+1, !whiteToMove, tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK, tEP, tWKC, tWQC, tBKC, tBQC);
                bestScore = std::max(bestScore, nextMoveScore);
                if (beta <= bestScore) 
                { 
                    // Move impossibly good (i.e. opponent blunder)
                    return bestScore;
                }
            }
        } 
        else 
        {
            // Check if legal move
            if (((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&tBK) == 0ULL) && (!illegalCastle))
            {
                float nextMoveScore = alphaBeta(alpha, bestScore, depth+1, !whiteToMove, tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK, tEP, tWKC, tWQC, tBKC, tBQC);
                bestScore = std::min(bestScore, nextMoveScore);
                if (bestScore <= alpha) 
                { 
                    // Move impossibly good (i.e. opponent blunder)
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
    int numChessmen;

    MAX_DEPTH = depth;

    numChessmen = __builtin_popcountll(WP|WR|WN|WB|WQ|WK|BP|BR|BN|BB|BQ|BQ);

    // Increase search depth if fewer chessmen on board
    if (numChessmen < 5)
        MAX_DEPTH += 4;
    else if (numChessmen < 10)
        MAX_DEPTH += 2;

    counter = 0; // FOR TESTING

    // Score each possible move (checking legality first)
    for (int i = 0; i < moveList.size(); i++) 
    {
        uint64_t tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK, tEP;
        bool tWKC = WKC, tWQC = WQC, tBKC = BKC, tBQC = BQC;
        bool illegalCastle = false;

        Move move = moveList.at(i);

        // Make the move
        tWP = moves.makeMove(move, WP, 1);
        tWR = moves.makeMove(move, WR, 2);
        tWN = moves.makeMove(move, WN, 3);
        tWB = moves.makeMove(move, WB, 4);
        tWQ = moves.makeMove(move, WQ, 5);
        tWK = moves.makeMove(move, WK, 6);
        tBP = moves.makeMove(move, BP, 1);
        tBR = moves.makeMove(move, BR, 2);
        tBN = moves.makeMove(move, BN, 3);
        tBB = moves.makeMove(move, BB, 4);
        tBQ = moves.makeMove(move, BQ, 5);
        tBK = moves.makeMove(move, BK, 6);
        tEP = 0ULL;

        /* Determine castling rights */
        if ((move.start&8574853690513424246ULL == 0ULL) || (move.dest&8574853690513424246ULL == 0ULL)) 
        { 
            // If move doesn't start or end at king or rook position
            if (move.special == 1) 
            { 
                // Castle
                if (whiteToMove && move.dest == 2ULL) 
                { 
                    // white kingside castle
                    if ((moves.attackedByBlack(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&14ULL) != 0ULL) 
                        illegalCastle = true;
                    else 
                    {
                        tWKC = false;
                        tWQC = false;
                    }
                } 
                else if (whiteToMove && move.dest == 32ULL) 
                { 
                    // white queenside castle
                    if ((moves.attackedByBlack(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&56ULL) != 0ULL)
                        illegalCastle = true;
                    else 
                    {
                        tWKC = false;
                        tWQC = false;
                    }
                } 
                else if ((!whiteToMove) && move.dest == 144115188075855872ULL) 
                { 
                    // black kingside castle
                    if ((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&1008806316530991104ULL) != 0ULL)
                        illegalCastle = true; 
                    else 
                    {
                        tBKC = false;
                        tBQC = false;
                    }
                } 
                else if ((!whiteToMove) && move.dest == 2305843009213693952ULL) 
                { 
                    // black queenside castle
                    if ((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&4035225266123964416ULL) != 0ULL)
                        illegalCastle = true;
                    else 
                    {
                        tBKC = false;
                        tBQC = false;
                    }
                }
            } 
            else if (move.start == 8ULL) 
            { 
                // White king moved
                tWKC = false;
                tWQC = false;
            } 
            else if (move.start == 576460752303423488ULL) 
            { 
                // Black king moved
                tBKC = false;
                tBQC = false;
            } 
            else if (move.start == 1ULL || move.dest == 1ULL) 
            { 
                // White kingside rook moved or captured
                tWKC = false;
            } 
            else if (move.start == 128ULL || move.dest == 128ULL) 
            { 
                // White queenside rook moved or captured
                tWQC = false;
            } 
            else if (move.start == 72057594037927936ULL || move.dest == 72057594037927936ULL) 
            { 
                // Black kingside rook moved or captured
                tBKC = false;
            } 
            else if (move.start == 9223372036854775808ULL || move.dest == 9223372036854775808ULL) 
            { 
                // Black queenside rook moved or captured
                tBQC = false;
            }
        } 
        else if ((move.chessman == 1)
            && (((move.start<<16) == move.dest) || ((move.start>>16) == move.dest))) 
        { 
            // En passant possibility
            tEP = move.dest;
        }

        // Check if legal move
        if (whiteToMove) 
        {
            if (((moves.attackedByBlack(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&tWK) == 0ULL) && (!illegalCastle))
            {
                // keep track of score for each move
                scores.at(i) = alphaBeta(-999, 999, 1, false, tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK, tEP, tWKC, tWQC, tBKC, tBQC);
            } 
            else 
            { 
                // illegal move
                scores.at(i) = -9999;
            }
        }
        else 
        {
            if (((moves.attackedByWhite(tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK)&tBK) == 0ULL) && (!illegalCastle)) 
            {
                // keep track of score for each move
                scores.at(i) = alphaBeta(-999, 999, 1, true, tWP, tWR, tWN, tWB, tWQ, tWK, tBP, tBR, tBN, tBB, tBQ, tBK, tEP, tWKC, tWQC, tBKC, tBQC);
            } 
            else 
            { 
                // illegal move
                scores.at(i) = 9999;
            }
        }
    }

    for (int i = 0; i < scores.size(); i++) // Print out each move and its score
        std::cout << moveToAlg(moveList.at(i)) << " " << scores.at(i) << std::endl;

    std::cout << "Nodes evaluated: " << counter << std::endl; // FOR TESTING

    // Choose best move score depending upon whose turn it is
    if (whiteToMove)
        target = *std::max_element(scores.begin(), scores.end());
    else
        target = *std::min_element(scores.begin(), scores.end());

    // Add all moves which have the best score
    for (int i = 0; i < scores.size(); i++) 
    {
        if (scores.at(i) == target)
            choices.push_back(moveList.at(i));
    }

    delete &target; // delete iterator (pointer)

    // Return best move (if multiple moves share the same score, choose one at random)
    int index = rand() % choices.size();
    return (choices.at(index));
}

std::string Search::moveToAlg(Move move)
{
    std::string algebra = "xxxxx"; // each char will change
    char File, Rank;

    int start = __builtin_ctzll(move.start);
    int dest = __builtin_ctzll(move.dest);

    File = start % 8;
    Rank = start / 8;

    algebra[0] = numToAlgFile(File);
    algebra[1] = Rank + 49;

    File = dest % 8;
    Rank = dest / 8;

    algebra[2] = numToAlgFile(File);
    algebra[3] = Rank + 49;

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

char Search::numToAlgFile(char file)
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

