#include "moves.h"

/* Binary reverse lookup table for byte */
uint64_t ReverseTable[256] = { R6(0), R6(2), R6(1), R6(3) };

Moves::Moves()
{
}

Moves::~Moves() //Destructor
{
}

uint64_t Moves::makeMove(Move theMove, uint64_t chessmen, int type)
{
    if (theMove.special == 0) { // "Normal" move
        if ((theMove.start&chessmen) != 0ULL) { // one of the given chessmen makes the move
            chessmen &= (~theMove.start); // remove original location
            chessmen |= (theMove.dest); // insert new location
        } else if ((theMove.dest&chessmen) != 0ULL) { // one of the given chessmen is captured
            chessmen &= (~theMove.dest); // remove chessman
        }
    } else if (theMove.special == 1) { // Castle
        if (theMove.dest == 2ULL) { // WHITE KINGSIDE
            if ((chessmen&8ULL) != 0ULL) { // king
                chessmen &= (~theMove.start);
                chessmen |= (theMove.dest);
            } else if ((chessmen&1ULL) != 0ULL) { //rook
                chessmen &= (~1ULL);
                chessmen |= (4ULL);
            }
        } else if (theMove.dest == 32ULL) { // WHITE QUEENSIDE
            if ((chessmen&8ULL) != 0ULL) { // king
                chessmen &= (~theMove.start);
                chessmen |= (theMove.dest);
            } else if ((chessmen&128ULL) != 0ULL) { //rook
                chessmen &= (~128ULL);
                chessmen |= (16ULL);
            }
        } else if (theMove.dest == 144115188075855872ULL) { // BLACK KINGSIDE
            if ((chessmen&576460752303423488ULL) != 0ULL) { // king
                chessmen &= (~theMove.start);
                chessmen |= (theMove.dest);
            } else if ((chessmen&72057594037927936ULL) != 0ULL) { //rook
                chessmen &= (~72057594037927936ULL);
                chessmen |= (288230376151711744ULL);
            }
        } else /* if (theMove.dest = 2305843009213693952ULL) */ { // BLACK QUEENSIDE
            if ((chessmen&576460752303423488ULL) != 0ULL) { // king
                chessmen &= (~theMove.start);
                chessmen |= (theMove.dest);
            } else if ((chessmen&9223372036854775808ULL) != 0ULL) { //rook
                chessmen &= (~9223372036854775808ULL);
                chessmen |= (1152921504606846976ULL);
            }
        }
    } else if (theMove.special == 2) { // En passant
        if ((theMove.start&chessmen) != 0ULL) { // are chessmen pawns?
            chessmen &= (~theMove.start);
            chessmen |= (theMove.dest);
        } else if (((theMove.dest>>8)&chessmen) != 0ULL) { // are chessmen victim of en passant?
            chessmen &= (~(theMove.dest>>8)); // remove chessman
        } else if (((theMove.dest<<8)&chessmen) != 0ULL) { // are chessmen victim of en passant?
            chessmen &= (~(theMove.dest<<8)); // remove chessman
        }
    } else if (theMove.special == 3) { // Promote to Queen
        if ((theMove.start&chessmen) != 0ULL) { // are chessmen pawns?
            chessmen &= (~theMove.start); // remove pawn
        } else if (type == 5 && ((theMove.dest&RANK_8) != 0ULL) && theMove.side == 1) { // white promotion
            chessmen |= (theMove.dest); // add queen
        } else if (type == 5 && ((theMove.dest&RANK_1) != 0ULL) && theMove.side == 2) { // black promotion
            chessmen |= (theMove.dest); // add queen
        } else {
            chessmen &= (~theMove.dest); //remove target
        }
    } else if (theMove.special == 4) { // Promote to Rook
        if ((theMove.start&chessmen) != 0ULL) { // are chessmen pawns?
            chessmen &= (~theMove.start); // remove pawn
        } else if (type == 2 && ((theMove.dest&RANK_8) != 0ULL) && theMove.side == 1) { // white promotion
            chessmen |= (theMove.dest); // add rook
        } else if (type == 2 && ((theMove.dest&RANK_1) != 0ULL) && theMove.side == 2) { // black promotion
            chessmen |= (theMove.dest); // add queen
        } else {
            chessmen &= (~theMove.dest); //remove target
        }
    } else if (theMove.special == 5) { // Promote to Bishop
        if ((theMove.start&chessmen) != 0ULL) { // are chessmen pawns?
            chessmen &= (~theMove.start); // remove pawn
        } else if (type == 4 && ((theMove.dest&RANK_8) != 0ULL) && theMove.side == 1) { // white promotion
            chessmen |= (theMove.dest); // add bishop
        } else if (type == 4 && ((theMove.dest&RANK_1) != 0ULL) && theMove.side == 2) { // black promotion
            chessmen |= (theMove.dest); // add queen
        } else {
            chessmen &= (~theMove.dest); //remove target
        }
    } else if (theMove.special == 6) { // Promote to Knight
        if ((theMove.start&chessmen) != 0ULL) { // are chessmen pawns?
            chessmen &= (~theMove.start); // remove pawn
        } else if (type == 3 && ((theMove.dest&RANK_8) != 0ULL) && theMove.side == 1) { // white promotion
            chessmen |= (theMove.dest); // add knight
        } else if (type == 3 && ((theMove.dest&RANK_1) != 0ULL) && theMove.side == 2) { // black promotion
            chessmen |= (theMove.dest); // add queen
        } else {
            chessmen &= (~theMove.dest); //remove target
        }
    } else {
        std::cout << "Something went terribly wrong (invalid move.special)" << std::endl;
    }
    return chessmen;
}

/*******************************/
/****** TESTING FUNCTIONS ******/
/*******************************/

void Moves::drawBoard(uint64_t WP, uint64_t WR, uint64_t WN, uint64_t WB, uint64_t WQ, uint64_t WK, uint64_t BP, uint64_t BR, uint64_t BN, uint64_t BB, uint64_t BQ, uint64_t BK)
{
    std::string chessBoard[8][8];
    for (int i=0; i<64; i++) {
        chessBoard[i/8][i%8] = "-";
    }
    for (int i=0; i<64; i++) {
        if      (((WP>>i)&1)==1) { chessBoard[i/8][i%8] = "P";} //white pawn
        else if (((WR>>i)&1)==1) { chessBoard[i/8][i%8] = "R";} //white rook
        else if (((WN>>i)&1)==1) { chessBoard[i/8][i%8] = "N";} //white knight
        else if (((WB>>i)&1)==1) { chessBoard[i/8][i%8] = "B";} //white bishop
        else if (((WQ>>i)&1)==1) { chessBoard[i/8][i%8] = "Q";} //white queen
        else if (((WK>>i)&1)==1) { chessBoard[i/8][i%8] = "K";} //white king
        else if (((BP>>i)&1)==1) { chessBoard[i/8][i%8] = "p";} //black pawn
        else if (((BR>>i)&1)==1) { chessBoard[i/8][i%8] = "r";} //black rook
        else if (((BN>>i)&1)==1) { chessBoard[i/8][i%8] = "n";} //black knight
        else if (((BB>>i)&1)==1) { chessBoard[i/8][i%8] = "b";} //black bishop
        else if (((BQ>>i)&1)==1) { chessBoard[i/8][i%8] = "q";} //black queen
        else if (((BK>>i)&1)==1) { chessBoard[i/8][i%8] = "k";} //black king
    }

    for (int i=0; i<64; i++) {
        std::cout << chessBoard[7-i/8][7-i%8] << " ";
        if (7-i%8 == 0)
            std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Moves::drawBitboard(uint64_t board)
{
    std::string chessBoard[8][8];
    for (int i=0; i<64; i++) {
        chessBoard[i/8][i%8] = "-";
    }
    for (int i=0; i<64; i++) {
        if (((board>>i)&1)==1) {
            chessBoard[i/8][i%8] = "X";
        }
    }
    for (int i=0; i<64; i++) {
        std::cout << chessBoard[7-i/8][7-i%8] << " ";
        if (7-i%8 == 0)
            std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Moves::toBinary(uint64_t num)
{
    for (int i = 0; i <64; i++)
        std::cout << (((num>>i)&1)==1);
    std::cout <<std::endl;
}

/*******************************/
/**** END TESTING FUNCTIONS ****/
/*******************************/

/* Reverse 64-bit binary using lookup table, one byte at a time */
uint64_t Moves::revBin(uint64_t n)
{
    uint64_t reversed;
    reversed = (ReverseTable[n & 255ULL] << 56) |
               (ReverseTable[(n >> 8) & 255ULL] << 48) |
               (ReverseTable[(n >> 16) & 255ULL] << 40) |
               (ReverseTable[(n >> 24) & 255ULL] << 32) |
               (ReverseTable[(n >> 32) & 255ULL] << 24) |
               (ReverseTable[(n >> 40) & 255ULL] << 16) |
               (ReverseTable[(n >> 48) & 255ULL] << 8) |
               (ReverseTable[(n >> 56) & 255ULL]);
    return reversed;
}

Move Moves::structGen(uint64_t s, uint64_t d, int c, int sp, int p)
{
    struct Move newMove;

    newMove.start = s;
    newMove.dest = d;
    newMove.chessman = c; // which piece is moving? 1 = pawn, 2 = rook, 3 = knight, 4 = bishop, 5 = queen, 6 = king
    newMove.special = sp; // 0 = normal, 1 = castle, 2 = en passant, 3 = promote queen, 4 = promote rook, 5 = promote bishop, 6 = promote knight
    newMove.priority = p; // captures and promotions have higher numbers
    newMove.side = sideToMove; // 1 = white, 2 = black

    return newMove;
}

/**************************************************************************/
/************** QUEEN, ROOK AND BISHOP HELPERS ****************************/
/**************************************************************************/

/* Formula: ( Occupied row  XOR ( occupied row - 2 * piece ) ) */
/* Need to reverse binary for opposite direction (right), then reverse back */
uint64_t Moves::HorizontalVerticalMoves(int loc)
{
    uint64_t binLoc = 1ULL<<loc;
    uint64_t horizontal = (OCCUPIED - (2ULL*binLoc)) ^ revBin(revBin(OCCUPIED) - (2ULL*revBin(binLoc)));
    uint64_t vertical = ((OCCUPIED&FILES[7-loc%8]) - (2ULL*binLoc)) ^ revBin(revBin(OCCUPIED&FILES[7-loc%8]) - (2ULL*revBin(binLoc)));

    return ((horizontal&RANKS[loc/8]) | (vertical&FILES[7-loc%8]));

}

uint64_t Moves::DiagonalMoves(int loc)
{
    uint64_t binLoc = 1ULL<<loc;
    uint64_t diagonal_right = ((OCCUPIED&DIAGONAL_SLANT_RIGHT[14-(loc/8 + loc%8)]) - (2ULL*binLoc))
                               ^ revBin(revBin(OCCUPIED&DIAGONAL_SLANT_RIGHT[14-(loc/8 + loc%8)]) - (2ULL*revBin(binLoc)));
    uint64_t diagonal_left = ((OCCUPIED&DIAGONAL_SLANT_LEFT[14-(loc/8 + (7-loc%8))]) - (2ULL*binLoc))
                               ^ revBin(revBin(OCCUPIED&DIAGONAL_SLANT_LEFT[14-(loc/8 + (7-loc%8))]) - (2ULL*revBin(binLoc)));

    return ((diagonal_right&DIAGONAL_SLANT_RIGHT[14-(loc/8 + loc%8)]) | (diagonal_left&DIAGONAL_SLANT_LEFT[14-(loc/8 + (7-loc%8))]));
}

/**************************************************************************/
/****************** ATTACKED SQUARES FUNCTIONS ****************************/
/**************************************************************************/

uint64_t Moves::attackedByWhite
(uint64_t WP, uint64_t WR, uint64_t WN, uint64_t WB, uint64_t WQ, uint64_t WK, uint64_t BP, uint64_t BR, uint64_t BN, uint64_t BB, uint64_t BQ, uint64_t BK)
{
    uint64_t attacked = 0ULL;
    uint64_t next = 0ULL;
    uint64_t moves = 0ULL;
    uint64_t QR = 0ULL;
    uint64_t QB = 0ULL;
    int loc;

    //Pawns
    attacked |= (WP<<7)&(~FILE_A); //capture right
    attacked |= (WP<<9)&(~FILE_H); //capture left

    // King
    next = (WK&~(WK-1ULL));
    loc = __builtin_ctzll(next);
    if (loc < 9) { // move KING_RANGE right
        moves = (KING_RANGE>>(9-loc));
    } else { // move KING_RANGE left
        moves = (KING_RANGE<<(loc-9));
    }
    if (loc%8 == 7) { // King on FILE A
        moves = moves&~FILE_H;
    } else if (loc%8 == 0) { // King on FILE H
        moves = moves&~FILE_A;
    }
    attacked |= moves;

    // Knights
    next = (WN&~(WN-1ULL));
    while (next != 0ULL) { // Go to each knight
        loc = __builtin_ctzll(next);
        if (loc < 18) { // move KNIGHT_RANGE right
            moves = (KNIGHT_RANGE>>(18-loc));
        } else { // move KNIGHT_RANGE left
            moves = (KNIGHT_RANGE<<(loc-18));
        }
        if (loc%8 < 2) { // Knight on FILE G-H
            moves = moves&~FILE_AB;
        } else if (loc%8 > 5) { // Knight on FILE A-B
            moves = moves&~FILE_GH;
        }
        attacked |= moves;
        WN = WN&~next;
        next = (WN&~(WN-1ULL));
    }

    // Queens/Rooks
    QR = (WQ|WR);
    next = (QR&~(QR-1ULL));
    while (next != 0ULL) { // Go to each piece
        moves = HorizontalVerticalMoves(__builtin_ctzll(next));

        attacked |= moves;

        QR = QR&~next;
        next = (QR&~(QR-1ULL));
    }

    // Queens/Bishops
    QB = (WQ|WB);
    next = (QB&~(QB-1ULL));
    while (next != 0ULL) { // Go to each piece
        moves = DiagonalMoves(__builtin_ctzll(next));

        attacked |= moves;

        QB = QB&~next;
        next = (QB&~(QB-1ULL));
    }

    return attacked;
}

uint64_t Moves::attackedByBlack
(uint64_t WP, uint64_t WR, uint64_t WN, uint64_t WB, uint64_t WQ, uint64_t WK, uint64_t BP, uint64_t BR, uint64_t BN, uint64_t BB, uint64_t BQ, uint64_t BK)
{
    uint64_t attacked = 0ULL;
    uint64_t next = 0ULL;
    uint64_t moves = 0ULL;
    uint64_t QR = 0ULL;
    uint64_t QB = 0ULL;
    int loc;

    //Pawns
    attacked |= (BP>>7)&(~FILE_H); //capture right
    attacked |= (BP>>9)&(~FILE_A); //capture left

    // King
    next = (BK&~(BK-1ULL));
    loc = __builtin_ctzll(next);
    if (loc < 9) { // move KING_RANGE right
        moves = (KING_RANGE>>(9-loc));
    } else { // move KING_RANGE left
        moves = (KING_RANGE<<(loc-9));
    }
    if (loc%8 == 7) { // King on FILE A
        moves = moves&~FILE_H;
    } else if (loc%8 == 0) { // King on FILE H
        moves = moves&~FILE_A;
    }
    attacked |= moves;

    // Knights
    next = (BN&~(BN-1ULL));
    while (next != 0ULL) { // Go to each knight
        loc = __builtin_ctzll(next);
        if (loc < 18) { // move KNIGHT_RANGE right
            moves = (KNIGHT_RANGE>>(18-loc));
        } else { // move KNIGHT_RANGE left
            moves = (KNIGHT_RANGE<<(loc-18));
        }
        if (loc%8 < 2) { // Knight on FILE G-H
            moves = moves&~FILE_AB;
        } else if (loc%8 > 5) { // Knight on FILE A-B
            moves = moves&~FILE_GH;
        }
        attacked |= moves;
        BN = BN&~next;
        next = (BN&~(BN-1ULL));
    }

    // Queens/Rooks
    QR = (BQ|BR);
    next = (QR&~(QR-1ULL));
    while (next != 0ULL) { // Go to each piece
        moves = HorizontalVerticalMoves(__builtin_ctzll(next));

        attacked |= moves;

        QR = QR&~next;
        next = (QR&~(QR-1ULL));
    }

    // Queens/Bishops
    QB = (BQ|BB);
    next = (QB&~(QB-1ULL));
    while (next != 0ULL) { // Go to each piece
        moves = DiagonalMoves(__builtin_ctzll(next));

        attacked |= moves;

        QB = QB&~next;
        next = (QB&~(QB-1ULL));
    }

    return attacked;
}

/**************************************************************************/
//**************** POSSIBLE MOVES FUNCTIONS *******************************/
/**************************************************************************/

/**************************************************/
/* POSSIBLE WHITE PAWN MOVES */
/**************************************************/

std::vector<Move> Moves::possibleWhitePawnMoves(uint64_t WP, uint64_t EP)
{
    std::vector<Move> stream;
    uint64_t moves = 0ULL;
    uint64_t nextMove = 0ULL;

    moves = (WP<<7)&ENEMY&~FILE_A&~RANK_8; // CAPTURE RIGHT
    nextMove = (moves&~(moves-1ULL));
    while(nextMove != 0ULL) {

        stream.push_back(structGen(nextMove>>7, nextMove, 1, 0, 5));

        moves = moves&~nextMove;
        nextMove = (moves&~(moves-1ULL));
    }

    moves = (WP<<9)&ENEMY&~FILE_H&~RANK_8; // CAPTURE LEFT
    nextMove = (moves&~(moves-1ULL));
    while(nextMove != 0ULL) {

        stream.push_back(structGen(nextMove>>9, nextMove, 1, 0, 5));

        moves = moves&~nextMove;
        nextMove = (moves&~(moves-1ULL));
    }

    moves = (WP<<8)&EMPTY&~RANK_8; // UP ONE
    nextMove = (moves&~(moves-1ULL));
    while(nextMove != 0ULL) {

        stream.push_back(structGen(nextMove>>8, nextMove, 1, 0, 0));

        moves = moves&~nextMove;
        nextMove = (moves&~(moves-1ULL));
    }

    moves = (WP<<16)&EMPTY&(EMPTY<<8)&RANK_4; // UP TWO
    nextMove = (moves&~(moves-1ULL));
    while(nextMove != 0ULL) {

        stream.push_back(structGen(nextMove>>16, nextMove, 1, 0, 0));

        moves = moves&~nextMove;
        nextMove = (moves&~(moves-1ULL));
    }

    moves = (WP<<8)&EMPTY&RANK_8; // PROMOTE UP ONE
    nextMove = (moves&~(moves-1ULL));
    while(nextMove != 0ULL) {

        stream.push_back(structGen(nextMove>>8, nextMove, 1, 3, 5));
        stream.push_back(structGen(nextMove>>8, nextMove, 1, 4, 5));
        stream.push_back(structGen(nextMove>>8, nextMove, 1, 5, 5));
        stream.push_back(structGen(nextMove>>8, nextMove, 1, 6, 5));

        moves = moves&~nextMove;
        nextMove = (moves&~(moves-1ULL));
    }

    moves = (WP<<7)&ENEMY&~FILE_A&RANK_8; // PROMOTE CAPTURE RIGHT
    nextMove = (moves&~(moves-1ULL));
    while(nextMove != 0ULL) {

        stream.push_back(structGen(nextMove>>7, nextMove, 1, 3, 6));
        stream.push_back(structGen(nextMove>>7, nextMove, 1, 4, 6));
        stream.push_back(structGen(nextMove>>7, nextMove, 1, 5, 6));
        stream.push_back(structGen(nextMove>>7, nextMove, 1, 6, 6));

        moves = moves&~nextMove;
        nextMove = (moves&~(moves-1ULL));
    }

    moves = (WP<<9)&ENEMY&~FILE_H&RANK_8; // PROMOTE CAPTURE LEFT
    nextMove = (moves&~(moves-1ULL));
    while(nextMove != 0ULL) {

        stream.push_back(structGen(nextMove>>9, nextMove, 1, 3, 6));
        stream.push_back(structGen(nextMove>>9, nextMove, 1, 4, 6));
        stream.push_back(structGen(nextMove>>9, nextMove, 1, 5, 6));
        stream.push_back(structGen(nextMove>>9, nextMove, 1, 6, 6));

        moves = moves&~nextMove;
        nextMove = (moves&~(moves-1ULL));
    }

    /* EN PASSANT */
    if (EP != 0ULL) { // Check for possible en passant square (EP = square to capture, NOT destination)

        nextMove = (WP<<7)&(EP<<8)&(~FILE_A); //EN PASSANT RIGHT
        if (nextMove != 0ULL) { // max of one en passant right

            stream.push_back(structGen(nextMove>>7, nextMove, 1, 2, 5));
        }

        nextMove = (WP<<9)&(EP<<8)&(~FILE_H); //EN PASSANT LEFT
        if (nextMove != 0ULL) { // max of one en passant left

            stream.push_back(structGen(nextMove>>9, nextMove, 1, 2, 5));
        }
    }

    return stream;
}

/**************************************************/
/* POSSIBLE BLACK PAWN MOVES */
/**************************************************/

std::vector<Move> Moves::possibleBlackPawnMoves(uint64_t BP, uint64_t EP)
{
    std::vector<Move> stream;
    uint64_t moves = 0ULL;
    uint64_t nextMove = 0ULL;

    moves = (BP>>7)&ENEMY&~FILE_H&~RANK_1; // CAPTURE RIGHT
    nextMove = (moves&~(moves-1ULL));
    while(nextMove != 0ULL) {

        stream.push_back(structGen(nextMove<<7, nextMove, 1, 0, 5));

        moves = moves&~nextMove;
        nextMove = (moves&~(moves-1ULL));
    }

    moves = (BP>>9)&ENEMY&~FILE_A&~RANK_1; // CAPTURE LEFT
    nextMove = (moves&~(moves-1ULL));
    while(nextMove != 0ULL) {

        stream.push_back(structGen(nextMove<<9, nextMove, 1, 0, 5));

        moves = moves&~nextMove;
        nextMove = (moves&~(moves-1ULL));
    }

    moves = (BP>>8)&EMPTY&~RANK_1; // UP ONE
    nextMove = (moves&~(moves-1ULL));
    while(nextMove != 0ULL) {

        stream.push_back(structGen(nextMove<<8, nextMove, 1, 0, 0));

        moves = moves&~nextMove;
        nextMove = (moves&~(moves-1ULL));
    }

    moves = (BP>>16)&EMPTY&(EMPTY>>8)&RANK_5; // UP TWO
    nextMove = (moves&~(moves-1ULL));
    while(nextMove != 0ULL) {

        stream.push_back(structGen(nextMove<<16, nextMove, 1, 0, 0));

        moves = moves&~nextMove;
        nextMove = (moves&~(moves-1ULL));
    }

    moves = (BP>>8)&EMPTY&RANK_1; // PROMOTE UP ONE
    nextMove = (moves&~(moves-1ULL));
    while(nextMove != 0ULL) {

        stream.push_back(structGen(nextMove<<8, nextMove, 1, 3, 5));
        stream.push_back(structGen(nextMove<<8, nextMove, 1, 4, 5));
        stream.push_back(structGen(nextMove<<8, nextMove, 1, 5, 5));
        stream.push_back(structGen(nextMove<<8, nextMove, 1, 6, 5));

        moves = moves&~nextMove;
        nextMove = (moves&~(moves-1ULL));
    }

    moves = (BP>>7)&ENEMY&~FILE_H&RANK_1; // PROMOTE CAPTURE RIGHT
    nextMove = (moves&~(moves-1ULL));
    while(nextMove != 0ULL) {

        stream.push_back(structGen(nextMove<<7, nextMove, 1, 3, 6));
        stream.push_back(structGen(nextMove<<7, nextMove, 1, 4, 6));
        stream.push_back(structGen(nextMove<<7, nextMove, 1, 5, 6));
        stream.push_back(structGen(nextMove<<7, nextMove, 1, 6, 6));

        moves = moves&~nextMove;
        nextMove = (moves&~(moves-1ULL));
    }

    moves = (BP>>9)&ENEMY&~FILE_A&RANK_1; // PROMOTE CAPTURE LEFT
    nextMove = (moves&~(moves-1ULL));
    while(nextMove != 0ULL) {

        stream.push_back(structGen(nextMove<<9, nextMove, 1, 3, 6));
        stream.push_back(structGen(nextMove<<9, nextMove, 1, 4, 6));
        stream.push_back(structGen(nextMove<<9, nextMove, 1, 5, 6));
        stream.push_back(structGen(nextMove<<9, nextMove, 1, 6, 6));

        moves = moves&~nextMove;
        nextMove = (moves&~(moves-1ULL));
    }

    /* EN PASSANT */
    if (EP != 0ULL) { // Check for possible en passant square (EP = square to capture, NOT destination)
        nextMove = (BP>>7)&(EP>>8)&(~FILE_H); //EN PASSANT RIGHT
        if (nextMove != 0ULL) { // max of one en passant right

            stream.push_back(structGen(nextMove<<7, nextMove, 1, 2, 5));
        }
        nextMove = (BP>>9)&(EP>>8)&(~FILE_A); //EN PASSANT LEFT
        if (nextMove != 0ULL) { // max of one en passant left

            stream.push_back(structGen(nextMove<<9, nextMove, 1, 2, 5));
        }
    }

    return stream;
}

/**************************************************/
/* POSSIBLE ROOK MOVES */
/**************************************************/

std::vector<Move> Moves::possibleRookMoves(uint64_t rooks)
{
    std::vector<Move> stream;
    uint64_t moves = 0ULL;
    uint64_t nextMove = 0ULL;
    uint64_t nextRook = 0ULL;

    nextRook = (rooks&~(rooks-1ULL));
    while (nextRook != 0ULL) { // Go to each rook
        moves = (CAPTURABLE)&HorizontalVerticalMoves(__builtin_ctzll(nextRook));
        nextMove = (moves&~(moves-1ULL));
        while(nextMove != 0ULL) { // Get each move

            if ((nextMove&ENEMY) == 0ULL) {
                stream.push_back(structGen(nextRook, nextMove, 2, 0, 0));
            } else {
                stream.push_back(structGen(nextRook, nextMove, 2, 0, 3));
            }

            moves = moves&~nextMove;
            nextMove = (moves&~(moves-1ULL));
        }
        rooks = rooks&~nextRook;
        nextRook = (rooks&~(rooks-1ULL));
    }

    return stream;
}

/**************************************************/
/* POSSIBLE BISHOP MOVES */
/**************************************************/

std::vector<Move> Moves::possibleBishopMoves(uint64_t bishops)
{
    std::vector<Move> stream;
    uint64_t moves = 0ULL;
    uint64_t nextMove = 0ULL;
    uint64_t nextBishop = 0ULL;

    nextBishop = (bishops&~(bishops-1ULL));
    while (nextBishop != 0ULL) { // Go to each bishop
        moves = (CAPTURABLE)&DiagonalMoves(__builtin_ctzll(nextBishop));
        nextMove = (moves&~(moves-1ULL));
        while(nextMove != 0ULL) { // Get each move

            if ((nextMove&ENEMY) == 0ULL) {
                stream.push_back(structGen(nextBishop, nextMove, 4, 0, 0));
            } else {
                stream.push_back(structGen(nextBishop, nextMove, 4, 0, 4));
            }


            moves = moves&~nextMove;
            nextMove = (moves&~(moves-1ULL));
        }
        bishops = bishops&~nextBishop;
        nextBishop = (bishops&~(bishops-1ULL));
    }

    return stream;
}

/**************************************************/
/* POSSIBLE QUEEN MOVES */
/**************************************************/

std::vector<Move> Moves::possibleQueenMoves(uint64_t queens)
{
    std::vector<Move> stream;
    uint64_t moves = 0ULL;
    uint64_t nextMove = 0ULL;
    uint64_t nextQueen = 0ULL;

    nextQueen = (queens&~(queens-1ULL));
    while (nextQueen != 0ULL) { // Go to each queen
        moves = (CAPTURABLE)&(DiagonalMoves(__builtin_ctzll(nextQueen)) | HorizontalVerticalMoves(__builtin_ctzll(nextQueen)));
        nextMove = (moves&~(moves-1ULL));
        while(nextMove != 0ULL) { // Get each move

            if ((nextMove&ENEMY) == 0ULL) {
                stream.push_back(structGen(nextQueen, nextMove, 5, 0, 0));
            } else {
                stream.push_back(structGen(nextQueen, nextMove, 5, 0, 2));
            }

            moves = moves&~nextMove;
            nextMove = (moves&~(moves-1ULL));
        }
        queens = queens&~nextQueen;
        nextQueen = (queens&~(queens-1ULL));
    }

    return stream;
}

/**************************************************/
/* POSSIBLE KNIGHT MOVES */
/**************************************************/

std::vector<Move> Moves::possibleKnightMoves(uint64_t knights)
{
    std::vector<Move> stream;
    uint64_t moves = 0ULL;
    uint64_t nextMove = 0ULL;
    uint64_t nextKnight = 0ULL;
    int loc;

    nextKnight = (knights&~(knights-1ULL));
    while (nextKnight != 0ULL) { // Go to each knight
        loc = __builtin_ctzll(nextKnight);
        if (loc < 18) { // move KNIGHT_RANGE right
            moves = (CAPTURABLE)&(KNIGHT_RANGE>>(18-loc));
        } else { // move KNIGHT_RANGE left
            moves = (CAPTURABLE)&(KNIGHT_RANGE<<(loc-18));
        }
        if (loc%8 < 2) { // Knight on FILE G-H
            moves = moves&(~FILE_AB);
        } else if (loc%8 > 5) { // Knight on FILE A-B
            moves = moves&(~FILE_GH);
        }
        nextMove = (moves&~(moves-1ULL));
        while(nextMove != 0ULL) { // Get each move

            if ((nextMove&ENEMY) == 0ULL) {
                stream.push_back(structGen(nextKnight, nextMove, 3, 0, 0));
            } else {
                stream.push_back(structGen(nextKnight, nextMove, 3, 0, 4));
            }

            moves = moves&~nextMove;
            nextMove = (moves&~(moves-1ULL));
        }
        knights = knights&~nextKnight;
        nextKnight = (knights&~(knights-1ULL));
    }

    return stream;
}

/**************************************************/
/* POSSIBLE KING MOVES */
/**************************************************/

std::vector<Move> Moves::possibleKingMoves(uint64_t king)
{
    std::vector<Move> stream;
    uint64_t moves = 0ULL;
    uint64_t nextMove = 0ULL;
    int loc;

    loc = __builtin_ctzll(king);
    if (loc < 9) { // move KING_RANGE right
        moves = (CAPTURABLE)&(KING_RANGE>>(9-loc));
    } else { // move KING_RANGE left
        moves = (CAPTURABLE)&(KING_RANGE<<(loc-9));
    }
    if (loc%8 == 7) { // King on FILE A
        moves = moves&~FILE_H;
    } else if (loc%8 == 0) { // King on FILE H
        moves = moves&~FILE_A;
    }
    nextMove = (moves&~(moves-1ULL));
    while(nextMove != 0ULL) { // Get each move

        if ((nextMove&ENEMY) == 0ULL) {
            stream.push_back(structGen(king, nextMove, 6, 0, 0));
        } else {
            stream.push_back(structGen(king, nextMove, 6, 0, 1));
        }

        moves = moves&~nextMove;
        nextMove = (moves&~(moves-1ULL));
    }

    return stream;
}

/**************************************************************************/
/* POSSIBLE CASTLING */
/**************************************************************************/

std::vector<Move> Moves::possibleWhiteCastle(bool kingside, bool queenside)
{
    std::vector<Move> stream;

    if (kingside && ((WHITE_KINGSIDE_CASTLE&EMPTY) == WHITE_KINGSIDE_CASTLE)) {
        stream.push_back(structGen(8ULL, 2ULL, 6, 1, 1));
    }

    if (queenside && ((WHITE_QUEENSIDE_CASTLE&EMPTY) == WHITE_QUEENSIDE_CASTLE)) {
        stream.push_back(structGen(8ULL, 32ULL, 6, 1, 1));
    }

    return stream;
}

std::vector<Move> Moves::possibleBlackCastle(bool kingside, bool queenside)
{
    std::vector<Move> stream;

    if (kingside && ((BLACK_KINGSIDE_CASTLE&EMPTY) == BLACK_KINGSIDE_CASTLE)) {
        stream.push_back(structGen(576460752303423488ULL, 144115188075855872ULL, 6, 1, 1));
    }

    if (queenside && ((BLACK_QUEENSIDE_CASTLE&EMPTY) == BLACK_QUEENSIDE_CASTLE)) {
        stream.push_back(structGen(576460752303423488ULL, 2305843009213693952ULL, 6, 1, 1));
    }

    return stream;
}

/**************************************************************************/
/* MAIN FUNCTIONS */
/**************************************************************************/

std::vector<Move> Moves::possibleMovesWhite
(uint64_t WP, uint64_t WR, uint64_t WN, uint64_t WB, uint64_t WQ, uint64_t WK, uint64_t BP, uint64_t BR, uint64_t BN, uint64_t BB, uint64_t BQ, uint64_t BK, uint64_t EP, bool WKC, bool WQC)
{
    std::vector<Move> temp;
    std::vector<Move> result;

    OCCUPIED = (WP|WR|WN|WB|WQ|WK|BP|BR|BN|BB|BQ|BK);
    EMPTY = ~OCCUPIED;
    ENEMY = (BP|BR|BN|BB|BQ);
    CAPTURABLE = ~(WP|WR|WN|WB|WQ|WK|BK);

    sideToMove = 1;

    temp = possibleWhiteCastle(WKC, WQC);
    if (temp.size() != 0)
        result.insert( result.end(), temp.begin(), temp.end() );

    temp = possibleBishopMoves(WB);
    if (temp.size() != 0)
        result.insert( result.end(), temp.begin(), temp.end() );

    temp = possibleKnightMoves(WN);
    if (temp.size() != 0)
        result.insert( result.end(), temp.begin(), temp.end() );

    temp = possibleQueenMoves(WQ);
    if (temp.size() != 0)
        result.insert( result.end(), temp.begin(), temp.end() );

    temp = possibleRookMoves(WR);
    if (temp.size() != 0)
        result.insert( result.end(), temp.begin(), temp.end() );

    temp = possibleWhitePawnMoves(WP, EP);
    if (temp.size() != 0)
        result.insert( result.end(), temp.begin(), temp.end() );

    temp = possibleKingMoves(WK);
    if (temp.size() != 0)
        result.insert( result.end(), temp.begin(), temp.end() );

    return result;
}

std::vector<Move> Moves::possibleMovesBlack
(uint64_t WP, uint64_t WR, uint64_t WN, uint64_t WB, uint64_t WQ, uint64_t WK, uint64_t BP, uint64_t BR, uint64_t BN, uint64_t BB, uint64_t BQ, uint64_t BK, uint64_t EP, bool BKC, bool BQC)
{
    std::vector<Move> temp;
    std::vector<Move> result;

    OCCUPIED = (WP|WR|WN|WB|WQ|WK|BP|BR|BN|BB|BQ|BK);
    EMPTY = ~OCCUPIED;
    ENEMY = (WP|WR|WN|WB|WQ);
    CAPTURABLE = ~(BP|BR|BN|BB|BQ|BK|WK);

    sideToMove = 2;

    temp = possibleBlackCastle(BKC, BQC);
    if (temp.size() != 0)
        result.insert( result.end(), temp.begin(), temp.end() );

    temp = possibleBishopMoves(BB);
    if (temp.size() != 0)
        result.insert( result.end(), temp.begin(), temp.end() );

    temp = possibleKnightMoves(BN);
    if (temp.size() != 0)
        result.insert( result.end(), temp.begin(), temp.end() );

    temp = possibleQueenMoves(BQ);
    if (temp.size() != 0)
        result.insert( result.end(), temp.begin(), temp.end() );

    temp = possibleRookMoves(BR);
    if (temp.size() != 0)
        result.insert( result.end(), temp.begin(), temp.end() );

    temp = possibleBlackPawnMoves(BP, EP);
    if (temp.size() != 0)
        result.insert( result.end(), temp.begin(), temp.end() );

    temp = possibleKingMoves(BK);
    if (temp.size() != 0)
        result.insert( result.end(), temp.begin(), temp.end() );

    return result;
}
