#pragma once
#include "utils.h"
#include "core/bitboard.hpp"

constexpr uint8_t COLOUR_MASK = 0b11000;
constexpr uint8_t TYPE_MASK = 0b00111;
constexpr int WHITE_BB_INDS[6] = {0, 1, 2, 3, 4, 5};
constexpr int BLACK_BB_INDS[6] = {6, 7, 8, 9, 10, 11};

                                                                    // Piece definitions
enum Piece : uint8_t
{
    // pieces represented as integers from binary with 2-bit colour and 3-bit type
    // white = 01XXX, black = 10XXX
    // piece types are XXX from PAWN = 1 -> KING = 6
    Empty = 0,

    WHITE = 0b01000,
    BLACK = 0b10000,

    PAWN = 0b00001,
    KNIGHT = 0b00010,
    BISHOP = 0b00011,
    ROOK = 0b00100,
    QUEEN = 0b00101,
    KING = 0b00110,

    WP = WHITE | PAWN,
    WN = WHITE | KNIGHT,
    WB = WHITE | BISHOP,
    WR = WHITE | ROOK,
    WQ = WHITE | QUEEN,
    WK = WHITE | KING,

    BP = BLACK | PAWN,
    BN = BLACK | KNIGHT,
    BB = BLACK | BISHOP,
    BR = BLACK | ROOK,
    BQ = BLACK | QUEEN,
    BK = BLACK | KING
};

using BitBoard = uint64_t;

struct Board
{
    Piece squares[64];      // what is on each square of the board
    BitB bitboards[12];     // bitboard for each piece and its positions

    bool white_to_move;     // who moves next

    // castling rights
    // this is for FEN ONLY -> does not tell you if castling is legal for a move
    // only tells you if castling is ever going to be legal for the remaining game
    bool white_king_side;
    bool white_queen_side;
    bool black_king_side;
    bool black_queen_side;

    // updated to cell index when en-passant available
    int en_passant_square;  // -1 if none

    int fullmove_number;    // starts at 1
};

                                                                    // Board setup / utility

// Set all squares to empty
// Reset side to move
// Clear castling rights
// Set en-passant square = -1
void clear_board(Board &board);

// Call clear_board
// Call load_start_position
void reset_board(Board &board);

                                                                    // Square helpers
// file: 0..7 for a..h
// rank: 0..7 for ranks 1..8

// returns int value of file : a..h -> 0..7
// must be lowercase from a,b,c,d,e,f,g,h
inline constexpr int char_file_to_int(char file)
{
    return file - 'a';
}

inline constexpr char int_file_to_char(int file)
{
    return 'a' + file;
}

// converts integer file/rank to board index
inline constexpr int file_rank_to_index(int file, int rank)
{
    return (rank * 8) + file;
}

// returns the file that board index is on
inline constexpr int index_to_file(int index)
{
    return index % 8;
}

// returns the rank that board index is on
inline constexpr int index_to_rank(int index)
{
    return index / 8;
}

inline constexpr bool is_valid_index(int index)
{
    return 0 <= index && index < 64;
}

inline constexpr bool is_valid_file_rank(int file, int rank)
{
    return 0 <= file && file <= 7 &&
           0 <= rank && rank <= 7;
}

inline std::string square_to_name(int square)
{
    if (!is_valid_index(square))
        return "-";

    int r = index_to_rank(square);
    char f = int_file_to_char(index_to_file(square));

    return std::string(1, f) + std::to_string(r + 1);
}

inline int name_to_square(const std::string& name)
{
    if (name.size() != 2)
        return -1;

    char fc = name[0];
    char rc = name[1];

    int f = char_file_to_int(fc);
    int r = rc - '1';

    int sq = file_rank_to_index(f, r);

    if (!is_valid_index(sq))
        return -1;

    return sq;
}

                                                                    // BitBoard helpers

// converts a Piece into bitboard index
// W: P=0, N=1, B=2, R=3, Q=4, K=5
// B: p=6, n=7, b=8, r=9, q=10, k=11
inline constexpr int piece_to_bb_ind(Piece piece)
{
    switch (piece)
    {
    case WP:
        return 0;
    case WN:
        return 1;
    case WB:
        return 2;
    case WR:
        return 3;
    case WQ:
        return 4;
    case WK:
        return 5;
    case BP:
        return 6;
    case BN:
        return 7;
    case BB:
        return 8;
    case BR:
        return 9;
    case BQ:
        return 10;
    case BK:
        return 11;
    default:
        return -1;
    }
}
                                                                    // Piece helpers

// true if piece is a white piece
inline constexpr bool is_white(Piece piece)
{
    return (uint8_t(piece) & COLOUR_MASK) == WHITE;
}

// true if piece is a black piece
inline constexpr bool is_black(Piece piece)
{
    return (uint8_t(piece) & COLOUR_MASK) == BLACK;
}

inline constexpr bool is_opponent(Piece piece1, Piece piece2)
{
    return piece1 != Empty &&
           piece2 != Empty &&
           is_white(piece1) != is_white(piece2);
}

// gets the int value of piece
inline constexpr Piece get_piece_type(Piece piece)
{
    return Piece(uint8_t(piece) & TYPE_MASK);
}

inline constexpr bool is_empty_p(Piece piece)
{
    return piece == Empty;
}

inline constexpr bool is_piece_on_board(const Board &board, Piece piece)
{
    return board.bitboards[piece_to_bb_ind(piece)] != 0;
}


void place_piece(Board &board, int square, Piece piece);
void remove_piece(Board &board, int square);
void move_piece(Board &board, int from, int to);

inline BitB white_occupancy(const Board &board);
inline BitB black_occupancy(const Board &board);
inline BitB all_occupancy(const Board &board);
inline BitB diagonal_attackers(const Board &board, bool white);
inline BitB straight_attackers(const Board &board, bool white);
inline int king_square(const Board &board, bool white);

                                                                    // Position loading

// Call a FEN loader with the start FEN
void load_start_position(Board &board);

// FEN helpers

char piece_to_char(Piece piece);

Piece char_to_piece(char c);
