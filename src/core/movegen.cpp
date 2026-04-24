#include "core/movegen.hpp"
#include "core/makemove.hpp"
#include "core/board.hpp"
#include "core/move.hpp"

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

const int KNIGHT_MOVES[8][2] = {{1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1}};
const int KING_MOVES[8][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
const int DIAGONAL_MOVES[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

bool is_in_check(const Board &board, bool white_king);
bool is_square_attacked(const Board &board, int square, bool by_white);
// finds king square for white[true] or black [false]
int find_king_square(const Board &board, bool white_king);

vector<Move> generate_pseudo_legal_moves(const Board &board)
{
    vector<Move> moves;

    for (int square = 0; square < 64; square++)
    {
        Piece piece = board.squares[square];

        if (piece == Empty)
            continue;

        // Skip enemy pieces
        if (board.white_to_move && is_black(piece))
            continue;

        if (!board.white_to_move && is_white(piece))
            continue;

        switch (get_piece_type(piece))
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
    Board copy = board; // create a test board

    // Apply the move to copy
    apply_move(copy, move);

    bool white_moved = is_white(move.piece);

    // If that side is now in check -> illegal move
    return !is_in_check(copy, white_moved);
}

// --------------------------------------------------
// Piece move generators
// --------------------------------------------------
/*
    TO DO:
    pawn - en passant, promotion
    king/rook - castling
*/

static void generate_pawn_moves(const Board &board, vector<Move> &moves, int square)
{
    Piece pawn = board.squares[square];
    bool white = is_white(pawn);
    int direction = white ? +8 : -8;
    int target = square + direction;
    int pawnrank = white ? 1 : 6;
    int pawnfile = index_to_file(square);
    int promotion_rank = white ? 7 : 0;

    if (!is_valid_index(target))
        return;

    Piece target_p = board.squares[target];
    if (is_empty_p(target_p))
    {
        if (index_to_rank(target) == promotion_rank)
        {
            Piece promotions[4] = {white ? WQ : BQ, white ? WR : BR, white ? WB : BB, white ? WN : BN};
            for (Piece promotion : promotions)
            {
                moves.push_back(Move{square, target, pawn, Empty, PROMOTION, promotion});
            }
        }
        else
        {
            moves.push_back(create_move(square, target, pawn, Empty));
        }

        target_p = board.squares[square + (2 * direction)];
        if (index_to_rank(square) == pawnrank && is_empty_p(target_p))
        {
            moves.push_back(Move{square, square + (2 * direction), pawn, Empty, DOUBLE_PAWN, Empty});
        }
    }

    // captures
    int leftright[2] = {-1, 1};
    for (int lr : leftright)
    {
        if (pawnfile + lr < 0 || pawnfile + lr > 7) // prevent capturing on opposite file of board
            continue;

        target_p = board.squares[target + lr];
        if (!is_empty_p(target_p) && is_opponent(pawn, target_p))
        {
            if (index_to_rank(target + lr) == promotion_rank)
            {
                Piece promotions[4] = {white ? WQ : BQ, white ? WR : BR, white ? WB : BB, white ? WN : BN};
                for (Piece promotion : promotions)
                {
                    moves.push_back(Move{square, target + lr, pawn, target_p, PROMO_CAPTURE, promotion});
                }
            }
            else
            {
                moves.push_back(create_move(square, target + lr, pawn, target_p));
            }
        }
    }
}

static void generate_knight_moves(const Board &board, vector<Move> &moves, int square)
{
    int start_file = index_to_file(square);
    int start_rank = index_to_rank(square);
    Piece knight = board.squares[square];

    for (int i = 0; i < 8; i++)
    {
        int tar_file = start_file + KNIGHT_MOVES[i][0];
        int tar_rank = start_rank + KNIGHT_MOVES[i][1];

        if (tar_rank < 0 || tar_rank > 7)
            continue;
        if (tar_file < 0 || tar_file > 7)
            continue;

        int tar_sq = file_rank_to_index(tar_file, tar_rank);
        Piece target = board.squares[tar_sq];

        if (is_empty_p(target))
            moves.push_back(create_move(square, tar_sq, knight, Empty));
        else if (is_opponent(knight, target))
            moves.push_back(create_move(square, tar_sq, knight, target));
    }
}

static void generate_bishop_moves(const Board &board, vector<Move> &moves, int square)
{
    Piece bishop = board.squares[square];
    int tar_sq;

    for (int i = 0; i < 4; i++) // loop through each diagonal direction from start square
    {
        int curr_file = index_to_file(square);
        int curr_rank = index_to_rank(square);

        while (true)
        {
            int tar_file = curr_file + DIAGONAL_MOVES[i][0];
            int tar_rank = curr_rank + DIAGONAL_MOVES[i][1];

            // stop if off board
            if (tar_file < 0 || tar_file > 7)
                break;
            if (tar_rank < 0 || tar_rank > 7)
                break;

            // target square and piece on that square
            tar_sq = file_rank_to_index(tar_file, tar_rank);
            Piece target = board.squares[tar_sq];

            if (is_empty_p(target))
            {
                moves.push_back(create_move(square, tar_sq, bishop, Empty));
            }

            else if (is_opponent(bishop, target))
            {
                moves.push_back(create_move(square, tar_sq, bishop, target));
                break;
            }

            // own piece blocks moves
            else
                break;

            curr_file = tar_file;
            curr_rank = tar_rank;
        }
    }
}

static void generate_rook_moves(const Board &board, vector<Move> &moves, int square)
{
    Piece rook = board.squares[square];
    int directions[2] = {1, -1};
    int start_file = index_to_file(square);
    int start_rank = index_to_rank(square);

    // moves along a rank
    for (int dir : directions)
    {
        int curr_file = start_file;

        while (true)
        {
            int tar_file = curr_file + dir;

            // stop if off board
            if (tar_file < 0 || tar_file > 7)
                break;

            // target square and piece on that square
            int tar_sq = file_rank_to_index(tar_file, start_rank);
            Piece target = board.squares[tar_sq];

            if (is_empty_p(target))
            {
                moves.push_back(create_move(square, tar_sq, rook, Empty));
            }

            else if (is_opponent(rook, target))
            {
                moves.push_back(create_move(square, tar_sq, rook, target));
                break;
            }

            // own piece blocks moves
            else
                break;

            curr_file = tar_file;
        }
    }

    // moves up and down a file
    for (int dir : directions)
    {
        int curr_rank = start_rank;

        while (true)
        {
            int tar_rank = curr_rank + dir;

            // stop if off board
            if (tar_rank < 0 || tar_rank > 7)
                break;

            // target square and piece on that square
            int tar_sq = file_rank_to_index(start_file, tar_rank);
            Piece target = board.squares[tar_sq];

            if (is_empty_p(target))
            {
                moves.push_back(create_move(square, tar_sq, rook, Empty));
            }

            else if (is_opponent(rook, target))
            {
                moves.push_back(create_move(square, tar_sq, rook, target));
                break;
            }

            // own piece blocks moves
            else
                break;

            curr_rank = tar_rank;
        }
    }
}

static void generate_queen_moves(const Board &board, vector<Move> &moves, int square)
{
    // Queen = bishop + rook movement
    generate_bishop_moves(board, moves, square);
    generate_rook_moves(board, moves, square);
}

static void generate_king_moves(const Board &board, vector<Move> &moves, int square)
{
    Piece king = board.squares[square];

    bool white = is_white(king);
    int start_file = index_to_file(square);
    int start_rank = index_to_rank(square);

    for (int i = 0; i < 8; i++)
    {
        int tar_file = start_file + KING_MOVES[i][0];
        int tar_rank = start_rank + KING_MOVES[i][1];

        if (tar_file < 0 || tar_file > 7)
            continue;
        if (tar_rank < 0 || tar_rank > 7)
            continue;

        int tar_sq = file_rank_to_index(tar_file, tar_rank);
        Piece tar_p = board.squares[tar_sq];

        if (is_empty_p(tar_p))
        {
            moves.push_back(create_move(square, tar_sq, king, Empty));
        }

        else if (is_opponent(king, tar_p))
        {
            moves.push_back(create_move(square, tar_sq, king, tar_p));
        }
    }

    // CASTLING
    if (king_can_castle_kingside(board, white))
        moves.push_back(
            create_move(square, white ? 6 : 62, king, Empty, KING_CASTLE));

    if (king_can_castle_queenside(board, white))
        moves.push_back(
            create_move(square, white ? 2 : 58, king, Empty, QUEEN_CASTLE));
}

int find_king_square(const Board &board, bool white_king)
{
    const Piece king = white_king ? WK : BK;

    for (int square = 0; square < 64; ++square)
    {
        if (board.squares[square] == king)
            return square;
    }

    return -1;
}

bool is_in_check(const Board &board, bool white_king)
{
    int king_square = find_king_square(board, white_king);
    if (king_square == -1)
        return false;

    if (white_king)
        return is_square_attacked(board, king_square, false);
    else
        return is_square_attacked(board, king_square, true);
}

bool is_square_attacked(const Board &board, int square, bool by_white)
{
    int file_check_attacked = index_to_file(square);
    int rank_check_attacked = index_to_rank(square);
    int tar_rank, tar_file;
    Piece tar_p{};

    // ------------------------------------
    // check opponent pawns attacking square
    // ------------------------------------

    tar_rank = by_white ? rank_check_attacked - 1 : rank_check_attacked + 1;
    int tar_files[2] = {file_check_attacked - 1, file_check_attacked + 1};
    for (int tf : tar_files)
    {
        if (tf < 0 || tf > 7)
            continue;

        if (tar_rank < 0 || tar_rank > 7)
            continue;

        tar_p = board.squares[file_rank_to_index(tf, tar_rank)];

        if (by_white && tar_p == WP)
            return true;

        if (!by_white && tar_p == BP)
            return true;
    }

    // ------------------------------------
    // opponent knight attacking square
    // ------------------------------------

    for (int i = 0; i < 8; i++)
    {
        tar_file = file_check_attacked + KNIGHT_MOVES[i][0];
        tar_rank = rank_check_attacked + KNIGHT_MOVES[i][1];

        if (tar_file < 0 || tar_file > 7)
            continue;

        if (tar_rank < 0 || tar_rank > 7)
            continue;

        tar_p = board.squares[file_rank_to_index(tar_file, tar_rank)];

        if (by_white && tar_p == WN)
            return true;

        if (!by_white && tar_p == BN)
            return true;
    }

    // ------------------------------------
    // opponent king attacks square
    // ------------------------------------

    for (int i = 0; i < 8; i++)
    {
        tar_file = file_check_attacked + KING_MOVES[i][0];
        tar_rank = rank_check_attacked + KING_MOVES[i][1];

        if (tar_file < 0 || tar_file > 7)
            continue;

        if (tar_rank < 0 || tar_rank > 7)
            continue;

        tar_p = board.squares[file_rank_to_index(tar_file, tar_rank)];

        if (by_white && tar_p == WK)
            return true;

        if (!by_white && tar_p == BK)
            return true;
    }

    // ------------------------------------
    // bishop/queen diagonal attacks
    // ------------------------------------

    for (int i = 0; i < 4; i++) // loop through each diagonal direction from start square
    {
        int curr_file = file_check_attacked;
        int curr_rank = rank_check_attacked;

        while (true)
        {
            int tar_file = curr_file + DIAGONAL_MOVES[i][0];
            int tar_rank = curr_rank + DIAGONAL_MOVES[i][1];

            // stop if off board - try another diagonal
            if (tar_file < 0 || tar_file > 7)
                break;
            if (tar_rank < 0 || tar_rank > 7)
                break;

            // target square and piece on that square
            tar_p = board.squares[file_rank_to_index(tar_file, tar_rank)];

            if (tar_p == Empty)
            {
                curr_file = tar_file;
                curr_rank = tar_rank;
                continue;
            }

            if (by_white && (tar_p == WB || tar_p == WQ))
                return true;

            if (!by_white && (tar_p == BB || tar_p == BQ))
                return true;

            break;
        }
    }

    // ------------------------------------
    // rook/queen straight attacks
    // ------------------------------------

    int directions[2] = {1, -1};

    // moves along a rank
    for (int dir : directions)
    {
        int curr_file = file_check_attacked;

        while (true)
        {
            int tar_file = curr_file + dir;

            // stop if off board
            if (tar_file < 0 || tar_file > 7)
                break;

            // target square and piece on that square
            tar_p = board.squares[file_rank_to_index(tar_file, rank_check_attacked)];

            if (tar_p == Empty)
            {
                curr_file = tar_file;
                continue;
            }

            if (by_white && (tar_p == WR || tar_p == WQ))
                return true;

            if (!by_white && (tar_p == BR || tar_p == BQ))
                return true;

            break;
        }
    }

    // moves up and down a file
    for (int dir : directions)
    {
        int curr_rank = rank_check_attacked;

        while (true)
        {
            int tar_rank = curr_rank + dir;

            // stop if off board
            if (tar_rank < 0 || tar_rank > 7)
                break;

            // target square and piece on that square
            tar_p = board.squares[file_rank_to_index(file_check_attacked, tar_rank)];

            if (tar_p == Empty)
            {
                curr_rank = tar_rank;
                continue;
            }

            if (by_white && (tar_p == WR || tar_p == WQ))
                return true;

            if (!by_white && (tar_p == BR || tar_p == BQ))
                return true;

            break;
        }
    }
    return false;
}

std::vector<Move> generate_legal_moves_for_square(const Board &board, int square)
{
    std::vector<Move> moves;

    for (const Move &move : generate_legal_moves(board))
    {
        if (move.from == square)
        {
            moves.push_back(move);
        }
    }

    return moves;
}

bool same_move(const Move &a, const Move &b)
{
    return a.from == b.from &&
           a.to == b.to &&
           a.piece == b.piece &&
           a.captured == b.captured &&
           a.flag == b.flag &&
           a.promotion == b.promotion;
}

bool king_can_castle_kingside(const Board &board, bool white)
{
    if ( // FEN has no kingside castling rights -> false
        white
            ? !board.white_king_side
            : !board.black_king_side)
        return false;

    if ( // king is not on start square -> false
        white
            ? !board.bitboards[piece_to_bb_ind(WK)][4]
            : !board.bitboards[piece_to_bb_ind(BK)][60])
        return false;

    if ( // kingside rook not on starting square -> false
         // rook move should update board.X_king_side anyway so just a safety measure
        white
            ? !board.bitboards[piece_to_bb_ind(WR)][7]
            : !board.bitboards[piece_to_bb_ind(BR)][63])
        return false;

    int squares_between_king_and_rook[2] = {
        white ? 5 : 61,
        white ? 6 : 62};

    for (int sq : squares_between_king_and_rook)
    {
        // squares between king and rook are not clear -> false
        if (board.squares[sq] != Empty)
            return false;

        // king castling through check -> false
        if (is_square_attacked(board, sq, !white))
            return false;
    }

    // currently in check -> false
    if (is_in_check(board, white))
        return false;

    return true;
}

bool king_can_castle_queenside(const Board &board, bool white)
{
    if ( // FEN has no queenside castling rights -> false
        white
            ? !board.white_queen_side
            : !board.black_queen_side)
        return false;

    if ( // king is not on start square -> false
        white
            ? !board.bitboards[piece_to_bb_ind(WK)][4]
            : !board.bitboards[piece_to_bb_ind(BK)][60])
        return false;

    if ( // queenside rook not on starting square -> false
         // rook move should update board.X_king_side anyway so just a safety measure
        white
            ? !board.bitboards[piece_to_bb_ind(WR)][0]
            : !board.bitboards[piece_to_bb_ind(BR)][56])
        return false;

    int squares_between_king_and_rook[3] =
        {
            white ? 1 : 57,
            white ? 2 : 58,
            white ? 3 : 59};

    for (int sq : squares_between_king_and_rook)
    {
        // squares between king and rook are not clear -> false
        if (board.squares[sq] != Empty)
            return false;

        // king castling through check -> false
        if (sq != 1 && sq != 57) // for queenside, ignore b1/b8
        {
            if (is_square_attacked(board, sq, !white))
                return false;
        }
    }

    // currently in check -> false
    if (is_in_check(board, white))
        return false;

    return true;
}