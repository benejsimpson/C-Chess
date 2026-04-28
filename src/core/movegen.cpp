#include "core/movegen.hpp"
#include "core/makemove.hpp"
#include "core/board.hpp"
#include "core/move.hpp"

#include <vector>
using namespace std;

// --------------------------------------------------
// Internal helpers
// --------------------------------------------------

static void generate_pawn_moves(const Board &board, MoveList &moves, int square);
static void generate_knight_moves(const Board &board, MoveList &moves, int square);
static void generate_bishop_moves(const Board &board, MoveList &moves, int square);
static void generate_rook_moves(const Board &board, MoveList &moves, int square);
static void generate_queen_moves(const Board &board, MoveList &moves, int square);
static void generate_king_moves(const Board &board, MoveList &moves, int square);
static bool is_legal_position_after_move(const Board &board, const Move &move);
bool is_in_check(const Board &board, bool white_king);

// returns true if square is attacked by enemy [piece_type]
static bool is_square_attacked_by_pawn(const Board &board, int square, bool by_white);
static bool is_square_attacked_by_knight(const Board &board, int square, bool by_white);
static bool is_square_attacked_by_diagonal(const Board &board, int square, bool by_white);
static bool is_square_attacked_by_straight(const Board &board, int square, bool by_white);
static bool is_square_attacked_by_king(const Board &board, int square, bool by_white);

bool is_square_attacked(const Board &board, int square, bool by_white);
static BitB capturable_opponent_occupancy(const Board &board, bool white);

bool king_can_castle_kingside(const Board &board, bool white);
bool king_can_castle_queenside(const Board &board, bool white);

MoveList generate_pseudo_legal_moves(const Board &board)
{
    MoveList moves;

    BitB pawns = board.bitboards[piece_to_bb_ind(board.white_to_move ? WP : BP)];
    while (pawns)
    {
        int from = pop_lsb(pawns);
        generate_pawn_moves(board, moves, from);
    }

    BitB knights = board.bitboards[piece_to_bb_ind(board.white_to_move ? WN : BN)];
    while (knights)
    {
        int from = pop_lsb(knights);
        generate_knight_moves(board, moves, from);
    }

    BitB bishops = board.bitboards[piece_to_bb_ind(board.white_to_move ? WB : BB)];
    while (bishops)
    {
        int from = pop_lsb(bishops);
        generate_bishop_moves(board, moves, from);
    }

    BitB rooks = board.bitboards[piece_to_bb_ind(board.white_to_move ? WR : BR)];
    while (rooks)
    {
        int from = pop_lsb(rooks);
        generate_rook_moves(board, moves, from);
    }

    BitB queens = board.bitboards[piece_to_bb_ind(board.white_to_move ? WQ : BQ)];
    while (queens)
    {
        int from = pop_lsb(queens);
        generate_queen_moves(board, moves, from);
    }

    BitB king = board.bitboards[piece_to_bb_ind(board.white_to_move ? WK : BK)];
    int from = lsb_index(king);
    if (from == -1)
        cout << "king not found\n";
    else
        generate_king_moves(board, moves, from);

    return moves;
}

// --------------------------------------------------
// Generate all fully legal moves
// --------------------------------------------------

MoveList generate_legal_moves(const Board &board)
{
    MoveList pseudo_moves = generate_pseudo_legal_moves(board);
    MoveList legal_moves;

    for (int i = 0; i < pseudo_moves.count; ++i)
    {
        Move move = pseudo_moves.moves[i];

        if (is_legal_position_after_move(board, move))
        {
            legal_moves.add(move);
        }
    }

    return legal_moves;
}

// Check whether a move leaves the side's king safe

static bool is_legal_position_after_move(const Board& board, const Move &move)
{
    Board copy = board;

    bool side_that_moved = copy.white_to_move;

    apply_move(copy, move);

    return !is_in_check(copy, side_that_moved);
}
// Piece move generators

static void generate_pawn_moves(const Board &board, MoveList &moves, int from)
{
    const bool white = board.white_to_move;
    const int rank = index_to_rank(from);
    const int direction = white ? 8 : -8;

    const BitB all_occup = all_occupancy(board);
    const BitB opp_occup = capturable_opponent_occupancy(board, white);

    const int one_forward = from + direction;
    const bool promotion_rank = rank == (white ? 6 : 1);

    // Forward move
    if (is_valid_index(one_forward) && !is_bit_set(all_occup, one_forward))
    {
        if (promotion_rank)
        {
            moves.add(create_move(from, one_forward, Q_PROMO));
            moves.add(create_move(from, one_forward, R_PROMO));
            moves.add(create_move(from, one_forward, B_PROMO));
            moves.add(create_move(from, one_forward, N_PROMO));
        }
        else
        {
            moves.add(create_move(from, one_forward, QUIET));

            const bool start_rank = rank == (white ? 1 : 6);
            const int two_forward = from + 2 * direction;

            if (start_rank && !is_bit_set(all_occup, two_forward))
            {
                moves.add(create_move(from, two_forward, DOUBLE_PAWN));
            }
        }
    }

    // Captures
    BitB attacks = white ? WHITE_PAWN_ATTACKS[from] : BLACK_PAWN_ATTACKS[from];

    while (attacks)
    {
        const int to = pop_lsb(attacks);

        if (to == board.en_passant_square)
        {
            moves.add(create_move(from, to, EN_PASSANT));
        }
        else if (is_bit_set(opp_occup, to))
        {
            if (promotion_rank)
            {
                moves.add(create_move(from, to, Q_PROMO));
                moves.add(create_move(from, to, R_PROMO));
                moves.add(create_move(from, to, B_PROMO));
                moves.add(create_move(from, to, N_PROMO));
            }
            else
            {
                moves.add(create_move(from, to, CAPTURE));
            }
        }
    }
}

static void generate_knight_moves(const Board &board, MoveList &moves, int from)
{
    const BitB all_occup = all_occupancy(board); // bb of all occupied squares
    const bool white = board.white_to_move; // piece on start square is a [white] piece
    const BitB opp_occup = capturable_opponent_occupancy(board, white);

    BitB const can_move_to_squares = KNIGHT_ATTACKS[from];
    BitB empty_squares_to_move_to = can_move_to_squares & ~all_occup;
    BitB squares_with_opponent_pieces = can_move_to_squares & opp_occup;

    while (empty_squares_to_move_to)
    {
        const int to = pop_lsb(empty_squares_to_move_to);
        moves.add(create_move(from, to, QUIET));
    }

    while (squares_with_opponent_pieces)
    {
        const int capture_square = pop_lsb(squares_with_opponent_pieces);
        moves.add(create_move(from, capture_square, CAPTURE));
    }
}

static void generate_bishop_moves(const Board &board, MoveList &moves, int square)
{
    const BitB all_occup = all_occupancy(board);    // bb of all occupied squares
    const bool white = board.white_to_move; // piece on start square is a [white] piece
    const BitB opp_occup = capturable_opponent_occupancy(board, white);

    for (int i = 0; i < 4; i++) // loop through each diagonal direction from start square
    {
        int curr_sq = square;
        while (true)
        {
            const int tar_sq = get_move_to_ind( // gets next square index on diagonal
                curr_sq,
                DIAGONAL_MOVES[i][0],
                DIAGONAL_MOVES[i][1]);

            if (tar_sq == -1) // if off board -> try another diagonal
                break;

            if (!is_bit_set(all_occup, tar_sq)) // empty square -> piece can move here
            {
                moves.add(create_move(square, tar_sq, QUIET));
            }

            else if (is_bit_set(opp_occup,tar_sq))
            {
                moves.add(create_move(square, tar_sq, CAPTURE));
                break;
            }

            // own piece blocks moves
            else
                break;

            curr_sq = tar_sq;
        }
    }
}

static void generate_rook_moves(const Board &board, MoveList &moves, int square)
{
    const BitB all_occup = all_occupancy(board);    // bb of all occupied squares
    const bool white = board.white_to_move; // piece on start square is a [white] piece
    const BitB opp_occup = capturable_opponent_occupancy(board, white);

    // moves along a rank
    for (int i = 0; i < 4; i++)
    {
        int curr_sq = square; // current square testing - init as start square
        const int d_file = STRAIGHT_MOVES[i][0];
        const int d_rank = STRAIGHT_MOVES[i][1];

        while (true)
        {
            const int tar_sq = get_move_to_ind(curr_sq, d_file, d_rank);

            // stop if off board
            if (tar_sq == -1)
                break;

            if (!is_bit_set(all_occup, tar_sq)) // no piece on square -> can make quiet move
            {
                moves.add(create_move(square, tar_sq, QUIET));
            }

            else if (is_bit_set(opp_occup,tar_sq)) // opponent piece on square -> can capture
            {
                moves.add(create_move(square, tar_sq, CAPTURE));
                break;
            }

            // own piece blocks moves
            else
                break;

            curr_sq = tar_sq;
        }
    }
}

static void generate_queen_moves(const Board &board, MoveList &moves, int square)
{
    // Queen = bishop + rook movement
    generate_bishop_moves(board, moves, square);
    generate_rook_moves(board, moves, square);
}

static void generate_king_moves(const Board &board, MoveList &moves, int from)
{
    const bool white = board.white_to_move;
    const BitB all_occup = all_occupancy(board);
    const BitB opp_occup = capturable_opponent_occupancy(board, white);

    BitB king_moves = KING_ATTACKS[from];

    while (king_moves)
    {
        const int to = pop_lsb(king_moves);
        if (is_bit_set(opp_occup, to))
            moves.add(create_move(from, to, CAPTURE));
        else if (!is_bit_set(all_occup, to))
            moves.add(create_move(from, to, QUIET));
    }

    // CASTLING
    if (king_can_castle_kingside(board, white))
        moves.add(
            create_move(from, white ? 6 : 62, KING_CASTLE));

    if (king_can_castle_queenside(board, white))
        moves.add(
            create_move(from, white ? 2 : 58, QUEEN_CASTLE));
}

bool is_in_check(const Board &board, bool white_king)
{
    int king_sq = king_square(board, white_king);
    if (king_sq == -1)
        return true;

    if (white_king)
        return is_square_attacked(board, king_sq, false);
    else
        return is_square_attacked(board, king_sq, true);
}

bool is_square_attacked(const Board &board, int square, bool by_white)
{
    if (!is_valid_index(square))
    {
        cout << "INVALID SQUARE in is_square_attacked : "<<square<<"\n";
        return false;
    }
    if (is_square_attacked_by_pawn(board, square, by_white))
        return true;
    if (is_square_attacked_by_knight(board, square, by_white))
        return true;
    if (is_square_attacked_by_diagonal(board, square, by_white))
        return true;
    if (is_square_attacked_by_straight(board, square, by_white))
        return true;
    if (is_square_attacked_by_king(board, square, by_white))
        return true;
    return false;
}

MoveList generate_legal_moves_for_square(const Board &board, int square)
{
    MoveList moves;
    MoveList all_moves = generate_legal_moves(board);

    for (int i = 0; i < all_moves.count; ++i)
    {
        Move move = all_moves.moves[i];

        if (move_from(move) == square)
        {
            moves.add(move);
        }
    }

    return moves;
}

bool same_move(const Move &a, const Move &b)
{
    return move_from(a) == move_from(b) &&
           move_to(a) == move_to(b) &&
           move_flag(a) == move_flag(b);
}

// returns true if [white] king can castle kingside
bool king_can_castle_kingside(const Board &board, bool white)
{
    if ( // FEN has no kingside castling rights -> false
         // updated when rook is moved so no need to check for rook on starting square
        white
            ? !board.white_king_side
            : !board.black_king_side)
        return false;

    if ( // king is not on start square -> false
        king_square(board, white) != (white ? 4 : 60))
        return false;

    if (!squares_between_king_and_rook_clear(board, white, true))
        return false;

    for (int sq : squares_to_check_between_king_and_rook(white, true))
    {
        // king castling through check -> false
        if (is_square_attacked(board, sq, !white))
            return false;
    }

    // currently in check -> false
    if (is_in_check(board, white))
        return false;

    return true;
}

// returns true if [white] king can castle queenside
bool king_can_castle_queenside(const Board &board, bool white)
{
    if ( // FEN has no queenside castling rights -> false
         // updated when rook is moved so no need to check for rook on starting square
        white
            ? !board.white_queen_side
            : !board.black_queen_side)
        return false;

    if ( // king is not on start square -> false
        king_square(board, white) != (white ? 4 : 60))
        return false;

    // squares between king and rook are not clear -> false
    if (!squares_between_king_and_rook_clear(board, white, false))
        return false;

    int squares_king_castles_through[2] =
        {
            white ? 2 : 58,
            white ? 3 : 59};

    for (int sq : squares_king_castles_through)
    {
        if (is_square_attacked(board, sq, !white))
            return false;
    }

    // currently in check -> false
    if (is_in_check(board, white))
        return false;

    return true;
}

static inline vector<int> squares_to_check_between_king_and_rook(bool white, bool kingside)
{
    vector<int> squares_to_check;
    if (kingside)
    {
        if (white)
            squares_to_check = {5, 6};
        else
            squares_to_check = {61, 62};
    }
    else
    {
        if (white)
            squares_to_check = {1, 2, 3};
        else
            squares_to_check = {57, 58, 59};
    }
    return squares_to_check;
}

static bool squares_between_king_and_rook_clear(const Board &board, bool white, bool kingside)
{
    vector<int> squares_to_check = squares_to_check_between_king_and_rook(white, kingside);
    for (int sq : squares_to_check)
    {
        if (is_bit_set(all_occupancy(board), sq))
            return false;
    }
    return true;
}

static bool is_square_attacked_by_pawn(const Board &board, int square, bool by_white)
{
    return (
        (by_white ? BLACK_PAWN_ATTACKS[square] : WHITE_PAWN_ATTACKS[square])
        & board.bitboards[piece_to_bb_ind(by_white ? WP : BP)])
        != EMPTY_BB;
}

static bool is_square_attacked_by_knight(const Board &board, int square, bool by_white)
{
    return (
        KNIGHT_ATTACKS[square]
        & board.bitboards[piece_to_bb_ind(by_white ? WN : BN)])
        != EMPTY_BB;
}

static bool is_square_attacked_by_diagonal(const Board &board, int square, bool by_white)
{
    if (diagonal_attackers(board, by_white) == EMPTY_BB)
        return false;

    for (int i = 0; i < 4; i++)
    {
        int curr_sq = square;

        while (true)
        {
            const int tar_sq = get_move_to_ind(curr_sq, DIAGONAL_MOVES[i][0], DIAGONAL_MOVES[i][1]);

            // stop if off board
            if (tar_sq == -1)
                break;

            if (!is_bit_set(all_occupancy(board), tar_sq)) // nothing on square -> next
            {
                curr_sq = tar_sq;
                continue;
            }

            if (is_bit_set(diagonal_attackers(board, by_white), tar_sq))
                return true;
            break;
        }
    }
    return false;
}
static bool is_square_attacked_by_straight(const Board &board, int square, bool by_white)
{
    if (straight_attackers(board, by_white) == EMPTY_BB)
        return false;

    const int directions[4][2] = {{0, -1}, {0, 1}, {1, 0}, {-1, 0}};

    for (int i = 0; i < 4; i++)
    {
        int curr_sq = square;

        while (true)
        {
            const int tar_sq = get_move_to_ind(curr_sq, directions[i][0], directions[i][1]);

            // stop if off board
            if (tar_sq == -1)
                break;

            if (!is_bit_set(all_occupancy(board), tar_sq)) // nothing on square -> next
            {
                curr_sq = tar_sq;
                continue;
            }

            if (is_bit_set(straight_attackers(board, by_white), tar_sq))
                return true;
            break;
        }
    }
    return false;
}

static bool is_square_attacked_by_king(const Board &board, int square, bool by_white)
{
    return (
        KING_ATTACKS[square]
        & board.bitboards[piece_to_bb_ind(by_white ? WK : BK)])
        != EMPTY_BB;
}

inline bool is_checkmate(Board &board)
{
    if (!is_in_check(board, board.white_to_move))
        return false;

    return generate_legal_moves(board).empty();
}

static BitB capturable_opponent_occupancy(const Board& board, bool white)
{
    BitB opp = white ? black_occupancy(board) : white_occupancy(board);

    // Kings are not capturable pieces.
    opp &= ~board.bitboards[piece_to_bb_ind(white ? BK : WK)];

    return opp;
}