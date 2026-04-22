#include "include/movegen.hpp"
#include "include/makemove.hpp"
#include "include/board.hpp"
#include "include/move.hpp"

#include <vector>
using namespace std;

// --------------------------------------------------
// Internal helper declarations
// These are only used inside this file
// --------------------------------------------------

static void generate_pawn_moves(const Board &board, vector<Move> &moves, int square);
static void generate_knight_moves(const Board &board, vector<Move> &moves, int square);
static void generate_bishop_moves(const Board &board, vector<Move> &moves, int square);
static void generate_rook_moves(const Board &board, vector<Move> &moves, int square);
static void generate_queen_moves(const Board &board, vector<Move> &moves, int square);
static void generate_king_moves(const Board &board, vector<Move> &moves, int square);

static bool is_legal_position_after_move(const Board &board, const Move &move);

// You will write this later
// It should return true if the side to move is currently in check
bool is_in_check(const Board &board);

// --------------------------------------------------
// Generate all pseudo-legal moves
// Pseudo-legal = follows piece movement rules,
// but may still leave your king in check
// --------------------------------------------------

vector<Move> generate_pseudo_legal_moves(const Board &board)
{
    vector<Move> moves;

    for (int square = 0; square < 64; square++)
    {
        Piece piece = board.squares[square];

        if (piece == Empty)
        {
            continue;
        }

        // Skip enemy pieces
        if (board.white_to_move && is_black(piece))
        {
            continue;
        }

        if (!board.white_to_move && is_white(piece))
        {
            continue;
        }

        switch (piece_type(piece))
        {
        case PAWN:
            generate_pawn_moves(board, moves, square);
            break;

        case KNIGHT:
            generate_knight_moves(board, moves, square);
            break;

        case BISHOP:
            generate_bishop_moves(board, moves, square);
            break;

        case ROOK:
            generate_rook_moves(board, moves, square);
            break;

        case QUEEN:
            generate_queen_moves(board, moves, square);
            break;

        case KING:
            generate_king_moves(board, moves, square);
            break;

        default:
            break;
        }
    }

    return moves;
}

// --------------------------------------------------
// Generate all fully legal moves
// Legal = pseudo-legal move that does not leave
// your own king in check
// --------------------------------------------------

vector<Move> generate_legal_moves(const Board &board)
{
    vector<Move> pseudo_moves = generate_pseudo_legal_moves(board);
    vector<Move> legal_moves;

    for (const Move &move : pseudo_moves)
    {
        if (is_legal_position_after_move(board, move))
        {
            legal_moves.push_back(move);
        }
    }

    return legal_moves;
}

// --------------------------------------------------
// Check whether a move leaves the side's king safe
// --------------------------------------------------

static bool is_legal_position_after_move(const Board &board, const Move &move)
{
    Board copy = board;

    make_move(copy, move);

    // After make_move(), side to move should switch.
    // So we need a way to check whether the player who
    // just moved has left their own king in check.
    //
    // For now, simplest version:
    // - if make_move flips white_to_move,
    //   then is_in_check(copy) should check the NEW side.
    // That means this function may need adjusting later
    // depending on how you write is_in_check().
    //
    // For now, keep the structure simple:
    return !is_in_check(copy);
}

// --------------------------------------------------
// Piece move generators
// Start with empty skeletons and fill them one by one
// --------------------------------------------------

static void generate_pawn_moves(const Board &board, vector<Move> &moves, int square)
// TO DO:
// - en passant
// - promotion

{
    Piece pawn = board.squares[square];
    bool white = is_white(pawn);
    int direction = white ? +8 : -8;
    int target = square + direction;
    int pawnrank = white ? 1 : 6;

    // moving forward 1 or 2 squares if on pawn rank
    Piece target_p = board.squares[target];
    if (is_empty_p(target_p))
    {
        moves.push_back(make_move(square, target, pawn));

        target_p = board.squares[square + (2 * direction)];
        if (index_to_rank(square) == pawnrank && is_empty_p(target_p))
        {
            moves.push_back(make_move(square, square + (2 * direction), pawn));
        }
    }

    // captures
    int leftright[2] = {-1, 1};
    for (int lr : leftright)
    {
        target_p = board.squares[target + lr];
        if (!is_empty_p(target_p) && index_to_rank(target + lr) == index_to_rank(target))
        {
            moves.push_back(make_move(square, target + lr, pawn));
        }
    }
}

        static void generate_knight_moves(const Board &board, vector<Move> &moves, int square)
        {
            // TODO:
            // test all 8 knight jump squares
            // if target is on board:
            //   - empty -> quiet move
            //   - enemy -> capture move
        }

        static void generate_bishop_moves(const Board &board, vector<Move> &moves, int square)
        {
            // TODO:
            // slide diagonally in 4 directions until:
            // - edge of board
            // - friendly piece blocks
            // - enemy piece can be captured, then stop
        }

        static void generate_rook_moves(const Board &board, vector<Move> &moves, int square)
        {
            // TODO:
            // slide horizontally/vertically in 4 directions
        }

        static void generate_queen_moves(const Board &board, vector<Move> &moves, int square)
        {
            // Queen = bishop + rook movement
            generate_bishop_moves(board, moves, square);
            generate_rook_moves(board, moves, square);
        }

        static void generate_king_moves(const Board &board, vector<Move> &moves, int square)
        {
            // TODO:
            // 1. normal king moves to 8 surrounding squares
            // 2. castling
        }

        const vector<int> horizontal_moves(int square)
        {
            vector<int> moves = {};
            int lower = (8 * (square / 8));
            int upper = (8 * (square / 8)) + 7;

            for (int s = lower; s <= upper; s++)
            {
                if (s == square)
                    continue;
                moves.push_back(s);
            }
            return moves;
        }

        const vector<int> verticle_pos_moves(int square)
        {
            vector<int> moves = {};
            int file = square % 8;
            int s;

            for (int rank = 0; rank < 8; rank++)
            {
                s = (8 * rank) + file;
                if (s == square)
                    continue;
                moves.push_back(s);
            }
            return moves;
        }

        const vector<int> diagonal_moves(int square)
        {
            vector<int> moves = {};
            int s = square + 9;
            while (s < 63)
            {
                moves.push_back(s);
                s += 9;
            }

            s = square - 7;

            for (int rank = 0; rank < 8; rank++)
            {
                int s = (8 * rank) + file;
                if (s == square)
                    continue;
                moves.push_back(s);
            }
            return moves;
        }