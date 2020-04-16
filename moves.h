#ifndef MOVES_H
#define MOVES_H
#include <iostream>
#include <cstdint>
#include <sstream>
#include <cmath>
#include <vector>

/* For binary reverse lookup table */
#define R2(n) n, n + 2*64, n + 1*64, n + 3*64
#define R4(n) R2(n), R2(n + 2*16), R2(n + 1*16), R2(n + 3*16)
#define R6(n) R4(n), R4(n + 2*4 ), R4(n + 1*4 ), R4(n + 3*4 )

struct Move 
{
    uint64_t start; // start location (bitboard)
    uint64_t dest; // destination
    int chessman; // 1 = pawn, 2 = rook, 3 = knight, 4 = bishop, 5 = queen, 6 = king
    int side; // 1 = white, 2 = black
    int special; // 0 = normal, 1 = castle, 2 = en passant, 3 = promote queen, 4 = promote rook, 5 = promote bishop, 6 = promote knight
    int priority;
};

class Moves
{
    public:
        Moves();
        ~Moves();

        uint64_t makeMove(Move move, uint64_t chessmen, int type); // type: 1 = pawn, 2 = rook, 3 = knight, 4 = bishop, 5 = queen, 6 = king

        std::vector<Move> possibleMovesWhite(uint64_t WP, uint64_t WR, uint64_t WN, uint64_t WB, uint64_t WQ, uint64_t WK, uint64_t BP, uint64_t BR, uint64_t BN, uint64_t BB, uint64_t BQ, uint64_t BK, uint64_t EP, bool WKC, bool WQC);
        std::vector<Move> possibleMovesBlack(uint64_t WP, uint64_t WR, uint64_t WN, uint64_t WB, uint64_t WQ, uint64_t WK, uint64_t BP, uint64_t BR, uint64_t BN, uint64_t BB, uint64_t BQ, uint64_t BK, uint64_t EP, bool BKC, bool BQC);

        uint64_t attackedByWhite(uint64_t WP, uint64_t WR, uint64_t WN, uint64_t WB, uint64_t WQ, uint64_t WK, uint64_t BP, uint64_t BR, uint64_t BN, uint64_t BB, uint64_t BQ, uint64_t BK);
        uint64_t attackedByBlack(uint64_t WP, uint64_t WR, uint64_t WN, uint64_t WB, uint64_t WQ, uint64_t WK, uint64_t BP, uint64_t BR, uint64_t BN, uint64_t BB, uint64_t BQ, uint64_t BK);


        /* Testing Functions */
        void drawBoard(uint64_t WP, uint64_t WR, uint64_t WN, uint64_t WB, uint64_t WQ, uint64_t WK, uint64_t BP, uint64_t BR, uint64_t BN, uint64_t BB, uint64_t BQ, uint64_t BK);
        void drawBitboard(uint64_t board);
        void toBinary(uint64_t num);

        /* BITBOARDS */
        /* Files */
        uint64_t FILES[8] = {9259542123273814144ULL, 4629771061636907072ULL, 2314885530818453536ULL, 1157442765409226768ULL, 578721382704613384ULL, 289360691352306692ULL, 144680345676153346ULL, 72340172838076673ULL};
        uint64_t FILE_A = 9259542123273814144ULL;
        uint64_t FILE_H = 72340172838076673ULL;
        uint64_t FILE_AB = 13889313184910721216ULL;
        uint64_t FILE_GH = 217020518514230019ULL;

        /* Ranks */
        uint64_t RANKS[8] = {255ULL, 65280ULL, 16711680ULL, 4278190080ULL, 1095216660480ULL, 280375465082880ULL, 71776119061217280ULL, 18374686479671623680ULL};
        uint64_t RANK_1 = 255ULL;
        uint64_t RANK_4 = 4278190080ULL;
        uint64_t RANK_5 = 1095216660480ULL;
        uint64_t RANK_8 = 18374686479671623680ULL;

        /* Diagonals */
         /* i.e. /, from upper left to bottom right */
        uint64_t DIAGONAL_SLANT_RIGHT[15] = {9223372036854775808ULL, 4647714815446351872ULL, 2323998145211531264ULL, 1161999622361579520ULL, 580999813328273408ULL, 290499906672525312ULL, 145249953336295424ULL, 72624976668147840ULL, 283691315109952ULL, 1108169199648ULL, 4328785936ULL, 16909320ULL, 66052ULL, 258ULL, 1ULL};
        /* i.e. \, from upper right to bottom left */
        uint64_t DIAGONAL_SLANT_LEFT[15] = {72057594037927936ULL, 144396663052566528ULL, 288794425616760832ULL, 577588855528488960ULL, 1155177711073755136ULL, 2310355422147575808ULL, 4620710844295151872ULL, 9241421688590303745ULL, 36099303471055874ULL, 141012904183812ULL, 550831656968ULL, 2151686160ULL, 8405024ULL, 32832ULL, 128ULL};

        /* Centers */
        uint64_t CENTER = 103481868288ULL;
        uint64_t EXTENDED_CENTER = 66229406269440ULL;
        uint64_t CENTER_BORDER = 66125924401152ULL;

        /* Sides */
        uint64_t KING_SIDE = 1085102592571150095ULL;
        uint64_t QUEEN_SIDE = 17361641481138401520ULL;

        uint64_t KING_RANGE = 460039ULL;
        uint64_t KNIGHT_RANGE = 43234889994ULL;

        /* Castling */
        uint64_t WHITE_KINGSIDE_CASTLE = 6ULL;
        uint64_t WHITE_QUEENSIDE_CASTLE = 112ULL;
        uint64_t BLACK_KINGSIDE_CASTLE = 432345564227567616ULL;
        uint64_t BLACK_QUEENSIDE_CASTLE = 8070450532247928832ULL;

    private:
        uint64_t revBin(uint64_t n); //reverse binary
        Move structGen(uint64_t s, uint64_t d, int c, int sp, int p);

        uint64_t HorizontalVerticalMoves(int loc);
        uint64_t DiagonalMoves(int loc);

        std::vector<Move> possibleWhitePawnMoves(uint64_t WP, uint64_t EP);
        std::vector<Move> possibleBlackPawnMoves(uint64_t BP, uint64_t EP);
        std::vector<Move> possibleRookMoves(uint64_t rooks);
        std::vector<Move> possibleBishopMoves(uint64_t bishops);
        std::vector<Move> possibleQueenMoves(uint64_t queens);
        std::vector<Move> possibleKnightMoves(uint64_t knights);
        std::vector<Move> possibleKingMoves(uint64_t kings);
        std::vector<Move> possibleWhiteCastle(bool kingside, bool queenside);
        std::vector<Move> possibleBlackCastle(bool kingside, bool queenside);

        uint64_t CAPTURABLE; // Enemy chessmen (minus King) and empty squares
        uint64_t OCCUPIED;
        uint64_t EMPTY;
        uint64_t ENEMY;

        int sideToMove; // 1 = white, 2 = black
};

#endif // MOVES_H
