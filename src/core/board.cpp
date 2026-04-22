#include "core/utils.h"
#include "core/board.hpp"
#include <cctype>

using namespace std;
using bit8 = uint8_t;
using bit64 = uint64_t;

const string FILES_CHAR = "abcdefgh";

// -------------------------
// Board setup / utility
// -------------------------

void reset_board(Board &board)
{
    // Call clear_board
    clear_board(board);

    // Call load_start_position
    load_start_position(board);
}

void clear_board(Board &board)
{
    // Set all squares to empty
    for (int i = 0; i < 64; i++)
    {
        board.squares[i] = Empty;
    }

    for (int i = 0; i < 12; i++)
    {
        board.bitboards[i].clear();
    }

    // Reset side to move
    board.white_to_move = true;

    // Clear castling rights
    board.white_king_side = false;
    board.white_queen_side = false;
    board.black_king_side = false;
    board.black_queen_side = false;

    // Set en-passant square = -1
    board.en_passant_square = -1;

    // Reset move counter
    board.fullmove_number = 1;
}

// -------------------------
// Piece helpers
// -------------------------

bool is_white(Piece piece)
{
    return (bit8(piece) & COLOUR_MASK) == WHITE;
}

bool is_black(Piece piece)
{
    return (bit8(piece) & COLOUR_MASK) == BLACK;
}

bit8 get_piece_type(Piece piece)
{
    return (bit8(piece)) & TYPE_MASK;
}

bool is_empty_p(Piece piece)
{
    return piece == Empty;
}

bool is_opponent(Piece my_piece, Piece test_piece)
{
    return is_white(my_piece) == is_black(test_piece);
}

// put a piece on a square
// updates board.squares[] and sets bit in piece's 64-bitboard
void place_piece(Board &board, int square, Piece piece)
{
    // set board.squares[square] to contain piece
    board.squares[square] = piece;

    // turn on bit in piece's 64-bitboard
    int bb_ind = piece_to_bitboard_index(piece);
    if (bb_ind != -1)
    {
        board.bitboards[bb_ind] += square;
    }
}

// removes a piece from a square
// updates board.squares[] and unsets bit in piece's 64-bitboard
void remove_piece(Board &board, int square, Piece piece)
{
    // set board.squares[square] to Empty
    board.squares[square] = Empty;

    // turn off bit in piece's 64-bitboard
    int bb_ind = piece_to_bitboard_index(piece);
    if (bb_ind != -1)
    {
        board.bitboards[bb_ind] -= square;
    }
}

// removes piece and places it in new position
// updates board.squares[] and piece's 64-bitboard
void move_piece(Board &board, int from, int to)
{
    // find the piece on the square moving from
    Piece piece = board.squares[from];

    // remove any captured piece on square
    Piece captured = board.squares[to];
    if (!is_empty_p(captured))
    {
        remove_piece(board, to, captured);
    }

    // remove moving piece from original position
    remove_piece(board, from, piece);

    // place piece on new square
    place_piece(board, to, piece);
}

// -------------------------
// Square helpers
// -------------------------

int char_file_to_int(char file)
{
    for (int f = 0; f < 8; f++)
    {
        if (FILES_CHAR.at(f) == file)
            return f;
    }
    return -1;
}

// converts INT file/rank to board index
// convert char file using `char_file_to_int()`
int file_rank_to_index(int file, int rank)
{
    return (rank * 8) + file;
}

int index_to_file(int index)
{
    return index % 8;
}

int index_to_rank(int index)
{
    return index / 8;
}

bool is_valid_index(int index, bool test)
{
    if (!(0 <= index && index < 64) && test)
    {
        cout << "INVALD INDEX : " << index << newl;
    }
    return 0 <= index && index < 64;
}

// -------------------------
// BitBoard helpers
// -------------------------

// converts a Piece into bitboard index WP->0, WN->1, ..., BK->11
int piece_to_bitboard_index(Piece piece)
{
    switch (piece)
    {
    case WP: return 0;
    case WN: return 1;
    case WB: return 2;
    case WR: return 3;
    case WQ: return 4;
    case WK: return 5;
    case BP: return 6;
    case BN: return 7;
    case BB: return 8;
    case BR: return 9;
    case BQ: return 10;
    case BK: return 11;
    default: return -1;
    }
}

bit64 square_to_bit(int square)
{
    return 1ULL << square;
}

// -------------------------
// Position loading
// -------------------------

void load_start_position(Board &board)
{
}

// -------------------------
// FEN helpers
// -------------------------

char piece_to_char(Piece piece)
{
    switch (piece)
    {
    case WP:
        return 'P';
    case WN:
        return 'N';
    case WB:
        return 'B';
    case WR:
        return 'R';
    case WQ:
        return 'Q';
    case WK:
        return 'K';

    case BP:
        return 'p';
    case BN:
        return 'n';
    case BB:
        return 'b';
    case BR:
        return 'r';
    case BQ:
        return 'q';
    case BK:
        return 'k';

    default:
        return '.';
    }
}

Piece char_to_piece(char c)
{
    switch (c)
    {
    case 'P':
        return WP;
    case 'N':
        return WN;
    case 'B':
        return WB;
    case 'R':
        return WR;
    case 'Q':
        return WQ;
    case 'K':
        return WK;

    case 'p':
        return BP;
    case 'n':
        return BN;
    case 'b':
        return BB;
    case 'r':
        return BR;
    case 'q':
        return BQ;
    case 'k':
        return BK;

    default:
        return Empty;
    }
}