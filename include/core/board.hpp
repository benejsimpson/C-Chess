/*
    Board Data
        - squares
        - side to move
        - castling rights
        - en passant
        - clocks?
    
    Helper Functions
        - clear board
        - reset board
        - square index helpers
        - piece type / colour helpers

    FEN Helpers -> FEN.
        - piece to char
        - char to piece
*/

#pragma once
#include "utils.h"

constexpr uint8_t COLOUR_MASK = 0b11000;
constexpr uint8_t TYPE_MASK   = 0b00111;


// -------------------------
// Piece definitions
// -------------------------

enum Piece : uint8_t
{
    // pieces represented as integers from binary with 2-bit colour and 3-bit type
    // white = 01XXX, black = 10XXX
    // piece types are XXX from PAWN = 1 -> KING = 6
    Empty = 0,

    WHITE = 0b01000,
    BLACK = 0b10000,

    PAWN   = 0b00001,
    KNIGHT = 0b00010,
    BISHOP = 0b00011,
    ROOK   = 0b00100,
    QUEEN  = 0b00101,
    KING   = 0b00110,


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

// -------------------------
// BitBoards
// -------------------------

struct BitBoard
{
    uint64_t bits = 0;

    // set bit at position
    // e.g. bb + 12
    BitBoard& operator+=(int square)
    {
        bits |= (1ULL << square);
        return *this;
    }

    // unset bit at position
    // e.g. bb - 12
    BitBoard& operator-=(int square)
    {
        bits &= ~(1ULL << square);
        return *this;
    }

    // check if a bit is turned on
    // e.g. if (bb[12])
    bool operator[](int square) const
    {
        return (bits & (1ULL << square)) != 0;
    }

    void clear()
    {
        bits = 0;
    }
};

// -------------------------
//          Board
// -------------------------

struct Board
{
    Piece squares[64]; // what is on each square of the board
    BitBoard bitboards[12]; // bitboard for each piece and its positions - oper: +=,-=,[]

    bool white_to_move; // who moves next

    //castling rights
    bool white_king_side;
    bool white_queen_side;
    bool black_king_side; 
    bool black_queen_side;

    // updated to cell index when en-passant available
    int en_passant_square;   // -1 if none

    int fullmove_number;     // starts at 1
};


// -------------------------
// Board setup / utility
// -------------------------

// Set all squares to empty
// Reset side to move
// Clear castling rights
// Set en-passant square = -1
void clear_board(Board& board);

// Call clear_board
// Call load_start_position
void reset_board(Board& board);


// -------------------------
// Square helpers
// -------------------------

int char_file_to_int(char file);
// file: 0..7 for a..h
// rank: 0..7 for ranks 1..8
int file_rank_to_index(int file, int rank);
int index_to_file(int index);
int index_to_rank(int index);
bool is_valid_index(int index, bool test = false);


// -------------------------
// BitBoard helpers
// -------------------------

int piece_to_bitboard_index(Piece piece);

uint64_t square_to_bit(int square);


// -------------------------
// Piece helpers
// -------------------------

// true if piece is a white piece
bool is_white(Piece piece);

// true if piece is a black piece
bool is_black(Piece piece);

bool is_opponent(Piece my_piece, Piece test_piece);

// gets the int value of piece
uint8_t get_piece_type(Piece piece);

bool is_empty_p(Piece piece);

void place_piece(Board& board, int square, Piece piece);
void remove_piece(Board& board, int square, Piece piece);
void move_piece(Board& board, int from, int to);

// --------------------------
// Move calculations
// --------------------------




// -------------------------
// Position loading
// -------------------------

// Call a FEN loader with the start FEN
void load_start_position(Board& board);


// -------------------------
// FEN helpers
// -------------------------

char piece_to_char(Piece piece);

Piece char_to_piece(char c);