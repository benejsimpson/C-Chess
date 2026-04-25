#include "core/utils.h"
#include "core/board.hpp"
#include "core/fen.hpp"
#include <cctype>

using namespace std;
using bit8 = uint8_t;
using bit64 = uint64_t;

// -------------------------
// Board setup / utility
// -------------------------

void reset_board(Board &board)
{
    clear_board(board);
    load_start_position(board);
}

void clear_board(Board &board)
{
    // set all squares to empty
    for (int i = 0; i < 64; i++)
    {
        board.squares[i] = Empty;
    }
    // clear bitboards for each piece
    for (int i = 0; i < 12; i++)
    {
        board.bitboards[i] = EMPTY_BB;
    }

    // reset side to move
    board.white_to_move = true;

    // clear castling rights
    board.white_king_side = false;
    board.white_queen_side = false;
    board.black_king_side = false;
    board.black_queen_side = false;

    // set en-passant square = -1
    board.en_passant_square = -1;

    // reset move counter
    board.fullmove_number = 1;
}

// -------------------------
// Piece helpers
// -------------------------

// put a piece on a square
// updates board.squares[] and piece's bitboard
void place_piece(Board &board, int square, Piece piece)
{
    if (!is_valid_index(square) || piece == Empty)
        return;

    board.squares[square] = piece;

    // update piece's 64-bitboard
    const int bb_ind = piece_to_bb_ind(piece);
    if (bb_ind != -1)
    {
        set_bit(board.bitboards[bb_ind], square);
    }
}

// removes a piece from a square
// updates board.squares[] and piece's bitboard
void remove_piece(Board &board, int square)
{
    if (!is_valid_index(square))
        return;

    const Piece piece = board.squares[square];
    if (piece == Empty)
        return;

    board.squares[square] = Empty;

    // unset bit in piece's bitboard
    clear_bit(
        board.bitboards[piece_to_bb_ind(piece)],
        square);
}

// removes piece and places it in new position
// updates board.squares[] and piece's bitboard
void move_piece(Board &board, int from, int to)
{
    // find the piece on the square moving from (the piece being moved)
    Piece piece = board.squares[from];

    // remove any captured piece on square moving to
    Piece captured = board.squares[to];
    if (!is_empty_p(captured))
    {
        remove_piece(board, to);
    }

    // remove moving piece from original position and place on new square
    remove_piece(board, from);
    place_piece(board, to, piece);
}


// -------------------------
// Bitboard helpers
// -------------------------

inline BitB white_occupancy(const Board &board)
{
    return board.bitboards[0] | board.bitboards[1] | board.bitboards[2] |
    board.bitboards[3] | board.bitboards[4] | board.bitboards[5];
}
inline BitB black_occupancy(const Board &board)
{
    return board.bitboards[6] | board.bitboards[7] | board.bitboards[8] |
    board.bitboards[9] | board.bitboards[10] | board.bitboards[11];
}
inline BitB all_occupancy(const Board &board)
{
    return white_occupancy(board) | black_occupancy(board);
}
inline int king_square(const Board& board, bool white)
{
    const BitB king_bb = board.bitboards[piece_to_bb_ind(white ? WK : BK)];
    return king_bb ? lsb_index(king_bb) : -1;
}

// -------------------------
// Position loading
// -------------------------

void load_start_position(Board &board)
{
    load_fen(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -");
    board.white_king_side = true;
    board.white_queen_side = true;
    board.black_king_side = true;
    board.black_queen_side = true;
}

// -------------------------
// FEN helpers
// -------------------------

// converts Piece p -> char representation in FEN
// white -> upper, black -> lower
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