#include "core/movegen.hpp"
#include "core/makemove.hpp"
#include "core/board.hpp"
#include "core/move.hpp"

#include <vector>
using namespace std;

                                                                                    // Constants and tables

// bitmask of squares that must be empty for white to castle kingside  
constexpr BitB WHITE_KINGSIDE_CASTLE_EMPTY_SQUARES = 1ULL << 5| 1ULL << 6;
// bitmask of squares that must be empty for white to castle queenside
constexpr BitB WHITE_QUEENSIDE_CASTLE_EMPTY_SQUARES = 1ULL << 1 | 1ULL << 2| 1ULL << 3;
// bitmask of squares that must be empty for black to castle kingside
constexpr BitB BLACK_KINGSIDE_CASTLE_EMPTY_SQUARES = 1ULL << 61 | 1ULL << 62;
// bitmask of squares that must be empty for black to castle queenside
constexpr BitB BLACK_QUEENSIDE_CASTLE_EMPTY_SQUARES = 1ULL << 57 | 1ULL << 58 | 1ULL << 59;
//bitmask of squares that must not be attacked for white to castle kingside
constexpr BitB WHITE_KINGSIDE_CASTLE_ATTACKED_SQUARES = 1ULL << 4 | 1ULL << 5 | 1ULL << 6;
// bitmask of squares that must not be attacked for white to castle queenside
constexpr BitB WHITE_QUEENSIDE_CASTLE_ATTACKED_SQUARES = 1ULL << 2 | 1ULL << 3 | 1ULL << 4;
// bitmask of squares that must not be attacked for black to castle kingside
constexpr BitB BLACK_KINGSIDE_CASTLE_ATTACKED_SQUARES = 1ULL << 60 | 1ULL << 61 | 1ULL << 62;
// bitmask of squares that must not be attacked for black to castle queenside
constexpr BitB BLACK_QUEENSIDE_CASTLE_ATTACKED_SQUARES = 1ULL << 58 | 1ULL << 59 | 1ULL << 60;

                                                                                    // Internal helpers

static void generate_pawn_moves(const Board &board, MoveList &moves, int square);
static void generate_knight_moves(const Board &board, MoveList &moves, int square);
static void generate_bishop_moves(const Board &board, MoveList &moves, int square);
static void generate_rook_moves(const Board &board, MoveList &moves, int square);
static void generate_queen_moves(const Board &board, MoveList &moves, int square);
static void generate_king_moves(const Board &board, MoveList &moves, int square);
static bool is_legal_position_after_move(const Board &board, const Move &move);
bool is_in_check(const Board &board, bool white_king);
static bool is_square_attacked_by_pawn(const Board &board, int square, bool by_white);
static bool is_square_attacked_by_knight(const Board &board, int square, bool by_white);
static bool is_square_attacked_by_diagonal(const Board &board, int square, bool by_white);
static bool is_square_attacked_by_straight(const Board &board, int square, bool by_white);
static bool is_square_attacked_by_king(const Board &board, int square, bool by_white);
bool is_square_attacked(const Board &board, int square, bool by_white);
static BitB capturable_opponent_occupancy(const Board &board, bool white);


                                                                                    // Move generation

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

MoveList generate_legal_moves_for_square(const Board &board, const int square)
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

                                                                                    // Piece move generation helpers

static bool is_legal_position_after_move(const Board& board, const Move &move)
{
    Board copy = board;

    bool side_that_moved = copy.white_to_move;

    apply_move(copy, move);

    return !is_in_check(copy, side_that_moved);
}

static void generate_pawn_moves(const Board &board, MoveList &moves, int from)
{
    const bool white = board.white_to_move;
    const int rank = index_to_rank(from);
    const int direction = white ? 8 : -8;

    const BitB all_occup = all_occupancy(board);
    const BitB capture_occup = capturable_opponent_occupancy(board, white);

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
        else if (is_bit_set(capture_occup, to))
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
    const BitB capture_occup = capturable_opponent_occupancy(board, white);

    BitB const can_move_to_squares = KNIGHT_ATTACKS[from];
    BitB empty_squares_to_move_to = can_move_to_squares & ~all_occup;
    BitB squares_with_opponent_pieces = can_move_to_squares & capture_occup;

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
    const bool white = board.white_to_move;         // piece on start square is a [white] piece
    const BitB capture_occup = capturable_opponent_occupancy(board, white);

    // loop through each diagonal direction from start square
    for (int i = 0; i < 4; i++) 
    {
        int curr_sq = square;
        while (true)
        {
            // get next square index on diagonal
            const int tar_sq = get_move_to_ind(
                curr_sq,
                DIAGONAL_MOVES[i][0],
                DIAGONAL_MOVES[i][1]);

            if (tar_sq == -1)                               // if off board -> try another diagonal
                break;

            if (!is_bit_set(all_occup, tar_sq))             // empty square -> piece can move here
            {
                moves.add(create_move(square, tar_sq, QUIET));
            }

            else if (is_bit_set(capture_occup,tar_sq))      // opponent piece on square -> can capture but no more moves on this diagonal
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
    const BitB all_occup = all_occupancy(board);            // bb of all occupied squares
    const bool white = board.white_to_move;                 // piece on start square is a [white] piece?
    const BitB capture_occup = capturable_opponent_occupancy(board, white);

    // loop through each straight direction from start square
    for (int i = 0; i < 4; i++)
    {
        int curr_sq = square;                               // current square being tested
        const int d_file = STRAIGHT_MOVES[i][0];
        const int d_rank = STRAIGHT_MOVES[i][1];

        while (true)
        {
            const int tar_sq = get_move_to_ind(curr_sq, d_file, d_rank);

            // stop if off board
            if (tar_sq == -1)
                break;

            if (!is_bit_set(all_occup, tar_sq))             // no piece on square -> can make quiet move
            {
                moves.add(create_move(square, tar_sq, QUIET));
            }

            else if (is_bit_set(capture_occup,tar_sq))      // opponent piece on square -> can capture
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
    const BitB capture_occup = capturable_opponent_occupancy(board, white);
    const BitB opp_attacks = white ? board.black_attacks : board.white_attacks;

    BitB king_moves = KING_ATTACKS[from];

    while (king_moves)
    {
        const int to = pop_lsb(king_moves);
        if (is_bit_set(capture_occup, to) && !is_bit_set(opp_attacks, to)) // can capture opponent piece if square not attacked by opponent
            moves.add(create_move(from, to, CAPTURE));
        else if (!is_bit_set(all_occup, to) && !is_bit_set(opp_attacks, to)) // can move to empty square if not attacked by opponent
            moves.add(create_move(from, to, QUIET));
    }

    generate_king_castles(board, moves, from);
}

                                                                                    // check / attack helpers

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

bool is_in_check(const Board &board, bool white_king)
{
    int king_sq = king_square(board, white_king);
    if (king_sq == -1)
    {
        cout << "ERROR: is_in_check returned king_square = -1\n";
        return true;
    }

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

bool same_move(const Move &a, const Move &b)
{
    return move_from(a) == move_from(b) &&
           move_to(a) == move_to(b) &&
           move_flag(a) == move_flag(b);
}

inline bool is_checkmate(Board &board)
{
    if (!is_in_check(board, board.white_to_move))
        return false;

    return generate_legal_moves(board).empty();
}

                                                                                    // NEW ATTACK MASKS

// generates bitboard of all squares [white] attacks - stores in board.[white/black]_attacks
// used for move generation and check detection
// this includes white attacking white pieces
// eg. if a white queen is blocked by a white knight, a bit will still be set at the position of the white knight
// hence, this mask can also be used to detect defended pieces
inline BitB generate_attack_masks_for_side(const Board &board, bool white)
{
    BitB attacks = 0;
    const BitB all_occup = all_occupancy(board);

    // Pawns
    BitB pawns = board.bitboards[piece_to_bb_ind(white ? WP : BP)];
    while (pawns)
    {
        int from = pop_lsb(pawns);
        attacks |= white ? WHITE_PAWN_ATTACKS[from] : BLACK_PAWN_ATTACKS[from];
    }

    // Knights
    BitB knights = board.bitboards[piece_to_bb_ind(white ? WN : BN)];
    while (knights)
    {
        int from = pop_lsb(knights);
        attacks |= KNIGHT_ATTACKS[from];
    }

    // Bishops
    BitB diagonals = diagonal_attackers(board,white);
    while (diagonals)
    {
        int from = pop_lsb(diagonals);
        for (auto& dir : DIAGONAL_MOVES)
        {
            int square = get_move_to_ind(from,dir[0],dir[1]);

            while (square != -1)
            {
                attacks |= square_mask(square);
                if (is_bit_set(all_occup,square))
                    break;
                square = get_move_to_ind(square,dir[0],dir[1]);
                
            }
        }
    }

    // Straight pieces (rooks + queens)
    BitB straights = straight_attackers(board, white);
    while (straights)
    {
        int from = pop_lsb(straights);
        for (auto& dir : STRAIGHT_MOVES)
        {
            int square = get_move_to_ind(from, dir[0], dir[1]);

            while (square != -1)
            {
                attacks |= square_mask(square);
                if (is_bit_set(all_occup,square))
                    break;
                square = get_move_to_ind(square, dir[0], dir[1]);
            }
        }
    }

    BitB king = board.bitboards[piece_to_bb_ind(white ? WK : BK)];
    int from = lsb_index(king);
    if (from == -1)
        cout << "ERROR generate_attack_masks_for_side : king not found\n";
    else
        attacks |= KING_ATTACKS[from];

    return attacks;
}

// updates board.[white/black]_attacks
// must be called after every move is made or when fen is loaded
inline void update_attack_masks(Board &board)
{
    board.white_attacks = generate_attack_masks_for_side(board, true);
    board.black_attacks = generate_attack_masks_for_side(board, false);
}

inline bool is_passed_pawn(const Board &board, int square, bool white)
{
    BitB enemy_pawns = white
        ? board.bitboards[piece_to_bb_ind(BP)]
        : board.bitboards[piece_to_bb_ind(WP)];

    BitB mask = white
        ? WHITE_PASSED_PAWN_MASKS[square]
        : BLACK_PASSED_PAWN_MASKS[square];

    return (enemy_pawns & mask) == EMPTY_BB;
}


                                                                                    // castling helpers

// generates legal castling moves - updates MoveList moves
static void generate_king_castles(const Board& board, MoveList& moves, int from)
{
    const bool white = board.white_to_move;
    const BitB all_occup = all_occupancy(board);
    const BitB rook_bb = board.bitboards[piece_to_bb_ind(white ? WR : BR)];
    const BitB opp_attacks = white ? board.black_attacks : board.white_attacks;

    // get kings square and squares of rooks on both sides
    const bool rook_can_castle_kingside = is_bit_set(rook_bb, (white ? 7 : 63));
    const bool rook_can_castle_queenside = is_bit_set(rook_bb, (white ? 0 : 56));

    if (from != (white ? 4 : 60))
        return;

    if (rook_can_castle_kingside && (white ? board.white_king_side : board.black_king_side))
    {
        // kingside castle squares that must be empty between king and rook
        const BitB kingside_squares_mask =
        white
        ? WHITE_KINGSIDE_CASTLE_EMPTY_SQUARES
        : BLACK_KINGSIDE_CASTLE_EMPTY_SQUARES;

        if ((kingside_squares_mask & all_occup) == 0) // squares between king and rook are empty :)
        {
            // kingside castle attacked squares
            const BitB kingside_attacked_squares_mask =
            white
            ? WHITE_KINGSIDE_CASTLE_ATTACKED_SQUARES
            : BLACK_KINGSIDE_CASTLE_ATTACKED_SQUARES;

            if ((kingside_attacked_squares_mask & opp_attacks) == 0) // squares king passes through are not attacked
                moves.add(create_move(from, (white ? 6 : 62), KING_CASTLE));
        }
    }

    if (rook_can_castle_queenside && (white ? board.white_queen_side : board.black_queen_side))
    {
        // queenside castle squares that must be empty between king and rook
        const BitB queenside_squares_mask =
        white
        ? WHITE_QUEENSIDE_CASTLE_EMPTY_SQUARES
        : BLACK_QUEENSIDE_CASTLE_EMPTY_SQUARES;

        if ((queenside_squares_mask & all_occup) == 0) // squares between king and rook are empty :)
        {
            // queenside castle attacked squares
            const BitB queenside_attacked_squares_mask =
            white
            ? WHITE_QUEENSIDE_CASTLE_ATTACKED_SQUARES
            : BLACK_QUEENSIDE_CASTLE_ATTACKED_SQUARES;

            if ((queenside_attacked_squares_mask & opp_attacks) == 0) // squares king passes through are not attacked
                moves.add(create_move(from, (white ? 2 : 58), QUEEN_CASTLE));
        }
    }
}

                                                                                    // other helpers

// bitboard of opponent pieces that can be captured by the side to move
// does not include opponent king since it cannot be captured
static BitB capturable_opponent_occupancy(const Board& board, bool white)
{
    BitB opp = white ? black_occupancy(board) : white_occupancy(board);

    // Kings are not capturable pieces.
    opp &= ~board.bitboards[piece_to_bb_ind(white ? BK : WK)];

    return opp;
}

