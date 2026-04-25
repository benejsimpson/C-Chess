#include "core/movegen.hpp"
#include "core/makemove.hpp"
#include "core/board.hpp"
#include "core/move.hpp"

#include <vector>
using namespace std;

// --------------------------------------------------
// Internal helpers
// --------------------------------------------------

static void generate_pawn_moves(const Board &board, vector<Move> &moves, int square);
static void generate_knight_moves(const Board &board, vector<Move> &moves, int square);
static void generate_bishop_moves(const Board &board, vector<Move> &moves, int square);
static void generate_rook_moves(const Board &board, vector<Move> &moves, int square);
static void generate_queen_moves(const Board &board, vector<Move> &moves, int square);
static void generate_king_moves(const Board &board, vector<Move> &moves, int square);
static bool is_legal_position_after_move(const Board &board, const Move &move);
bool is_in_check(const Board &board, bool white_king);

const int KNIGHT_MOVES[8][2] = {{1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1}};
const int KING_MOVES[8][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
const int DIAGONAL_MOVES[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

// returns true if square is attacked by enemy [piece_type]
static bool is_square_attacked_by_pawn(const Board &board, int square, bool by_white);
static bool is_square_attacked_by_knight(const Board &board, int square, bool by_white);
static bool is_square_attacked_by_diagonal(const Board &board, int square, bool by_white);
static bool is_square_attacked_by_straight(const Board &board, int square, bool by_white);
static bool is_square_attacked_by_king(const Board &board, int square, bool by_white);

bool is_square_attacked(const Board &board, int square, bool by_white);


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
    const bool white = is_white(pawn);
    const int d_rank = white ? +1 : -1; // rank displacement of single step forward
    const int start_file = index_to_file(square); // the file that the pawn is on before moving
    const int start_rank = index_to_rank(square); // the rank that the pawn is on before moving
    const int home_rank = white ? 1 : 6; // if the pawn is on this rank then it can double move
    const int promotion_rank = white ? 7 : 0; // if the moving to is this, then the pawn can promote
    const int target_sq = get_move_to_ind(square,0,d_rank);

    const int direction = white ? +8 : -8;

    if (target_sq == -1) // single step forward puts pawn on invalid square - this should not happen realistically
        return;

    if (!is_bit_set(all_occupancy(board), target_sq)) // there is nothing directly in front of the pawn
    {
        // PROMOTION
        if (start_rank + d_rank == promotion_rank) // pawn can move onto the promotion rank
        {
            Piece promotions[4] = {white ? WQ : BQ, white ? WR : BR, white ? WB : BB, white ? WN : BN};
            for (Piece promotion : promotions)
            {
                moves.push_back(Move{square, target_sq, pawn, Empty, PROMOTION, promotion});
            }
        }

        // SINGLE STEP FORWARD
        else
        {
            moves.push_back(create_move(square, target_sq, pawn, Empty));
        }

        // DOUBLE STEP FORWARD
        const int target_sq_2step = get_move_to_ind(square, 0, 2 * d_rank);

        if (start_rank == home_rank && !is_bit_set(all_occupancy(board),target_sq_2step)) // pawn is on home rank and no piece on square 2 ahead
        {
            moves.push_back( Move{ square, target_sq_2step, pawn, Empty, DOUBLE_PAWN, Empty } );
        }
    }

    // CAPTURES
    int leftright[2] = {-1, 1};
    for (int lr : leftright)
    {
        const int att_sq = get_move_to_ind(square, lr, d_rank);
        if (att_sq == -1)
            continue;
        
        if (att_sq == board.en_passant_square)
        {
            moves.push_back(
                Move{
                    square,
                    att_sq,
                    pawn,
                    board.squares[att_sq + (white ? -8 : 8)],
                    EN_PASSANT
                }
            );
        }

        // if no opponent piece on square -> continue
        if (!is_bit_set(
            white ? black_occupancy(board) : white_occupancy(board),
            att_sq)
            )
            continue;

        if (index_to_rank(target_sq) == promotion_rank)
        {
            Piece promotions[4] = {white ? WQ : BQ, white ? WR : BR, white ? WB : BB, white ? WN : BN};
            for (Piece promotion : promotions)
            {
                moves.push_back(
                    Move{
                        square,
                        target_sq,
                        pawn,
                        board.squares[att_sq],
                        PROMO_CAPTURE,
                        promotion
                    }
                );
            }
        }
        else
        {
            moves.push_back(
                create_move(
                    square,
                    att_sq,
                    pawn,
                    board.squares[att_sq]
                )
            );
        }
    }
}

static void generate_knight_moves(const Board &board, vector<Move> &moves, int square)
{
    int start_file = index_to_file(square);
    int home_rank = index_to_rank(square);
    Piece knight = board.squares[square];

    for (int i = 0; i < 8; i++)
    {
        int tar_file = start_file + KNIGHT_MOVES[i][0];
        int tar_rank = home_rank + KNIGHT_MOVES[i][1];

        if (!is_valid_file_rank(tar_file, tar_rank))
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
            if (!is_valid_file_rank(tar_file, tar_rank))
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
    int home_rank = index_to_rank(square);

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
            int tar_sq = file_rank_to_index(tar_file, home_rank);
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
        int curr_rank = home_rank;

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

    for (int i = 0; i < 8; i++)
    {
        const int to_sq = get_move_to_ind(square, KING_MOVES[i][0], KING_MOVES[i][1]);

        if (to_sq == -1)
            continue;

        // NORMAL MOVE
        if (!is_bit_set(all_occupancy(board),to_sq))
        {
            moves.push_back(create_move(square, to_sq, king, Empty));
        }

        // CAPTURE
        else if (is_bit_set(white ? black_occupancy(board) : white_occupancy(board), to_sq))
        {
            moves.push_back(create_move(square, to_sq, king, board.squares[to_sq]));
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

bool is_in_check(const Board &board, bool white_king)
{
    int king_sq = king_square(board, white_king);
    if (king_sq == -1)
        return false;

    if (white_king)
        return is_square_attacked(board, king_sq, false);
    else
        return is_square_attacked(board, king_sq, true);
}

bool is_square_attacked(const Board &board, int square, bool by_white)
{
    if (is_square_attacked_by_pawn(board,square,by_white))
        return true;
    if (is_square_attacked_by_knight(board,square,by_white))
        return true;
    if (is_square_attacked_by_diagonal(board,square,by_white))
        return true;
    if (is_square_attacked_by_straight(board,square,by_white))
        return true;
    if (is_square_attacked_by_king(board,square,by_white))
        return true;
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
         // updated when rook is moved so no need to check for rook on starting square
        white
            ? !board.white_king_side
            : !board.black_king_side)
        return false;

    if ( // king is not on start square -> false
        king_square(board,white) != white ? 4 : 60)
        return false;

    if (!squares_between_king_and_rook_clear(board,white,true))
        return false;

    int squares_between_king_and_rook[2] = {
        white ? 5 : 61,
        white ? 6 : 62};

    for (int sq : squares_to_check_between_king_and_rook(white,true))
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

bool king_can_castle_queenside(const Board &board, bool white)
{
    if ( // FEN has no queenside castling rights -> false
         // updated when rook is moved so no need to check for rook on starting square
        white
            ? !board.white_queen_side
            : !board.black_queen_side)
        return false;

    if ( // king is not on start square -> false
        king_square(board,white) != white ? 4 : 60)
        return false;

    // squares between king and rook are not clear -> false
    if (!squares_between_king_and_rook_clear(board,white,false))
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

static bool squares_between_king_and_rook_clear(const Board& board, bool white, bool kingside)
{
    vector<int> squares_to_check = squares_to_check_between_king_and_rook(white,kingside);
    for (int sq : squares_to_check)
    {
        if (is_bit_set(all_occupancy(board),sq))
            return false;
    }
    return true;
}


static bool is_square_attacked_by_pawn(const Board &board, int square, bool by_white)
{
    if (board.bitboards[piece_to_bb_ind(by_white ? WP : BP)] == EMPTY_BB) // no opponent pawns on board -> false
        return false;

    int att_d_rank = by_white ? - 1 : 1;    // rank of potential attacking pawns
    int att_d_files[2] = {- 1,  1};         // files of potential attacking pawns

    // bitboard index of piece that we are checking is attacking the square
    int att_p_bb = by_white ? piece_to_bb_ind(WP) : piece_to_bb_ind(BP);

    for (int att_d_file : att_d_files)
    {
        const int att_sq = get_move_to_ind(square, att_d_file, att_d_rank); // square of potential attacking pawn

        if (att_sq == -1)
            continue;

        if (is_bit_set(board.bitboards[att_p_bb], att_sq)) // if opponent pawn on that square -> true
            return true;
    }
    return false;
}

static bool is_square_attacked_by_knight(const Board &board, int square, bool by_white)
{
    if (board.bitboards[piece_to_bb_ind(by_white ? WN : BN)] == EMPTY_BB) // no opponent knights on board -> false
        return false;

    // bitboard index of piece that we are checking is attacking the square
    int att_p_bb = by_white ? piece_to_bb_ind(WN) : piece_to_bb_ind(BN);

    for (int i = 0; i < 8; i++)
    {
        const int to_sq = get_move_to_ind(square, KNIGHT_MOVES[i][0], KNIGHT_MOVES[i][1]);

        if (to_sq == -1)
            continue;

        if (is_bit_set(board.bitboards[att_p_bb],to_sq))
            return true;
    }
    return false;
}

static bool is_square_attacked_by_diagonal(const Board &board, int square, bool by_white)
{
    if (diagonal_attackers(board,by_white) == EMPTY_BB)
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

            if (!is_bit_set(all_occupancy(board),tar_sq)) // nothing on square -> next
            {
                curr_sq = tar_sq;
                continue;
            }
            
            if (is_bit_set(diagonal_attackers(board, by_white),tar_sq))
                return true;
            break;
        }
    }
    return false;
}
static bool is_square_attacked_by_straight(const Board &board, int square, bool by_white)
{
    if (straight_attackers(board,by_white) == EMPTY_BB)
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

            if (!is_bit_set(all_occupancy(board),tar_sq)) // nothing on square -> next
            {
                curr_sq = tar_sq;
                continue;
            }
            
            if (is_bit_set(straight_attackers(board, by_white),tar_sq))
                return true;
            break;
        }
    }
    return false;
}

static bool is_square_attacked_by_king(const Board &board, int square, bool by_white)
{
    int att_p_bb = by_white ? piece_to_bb_ind(WK) : piece_to_bb_ind(BK);

    for (int i = 0; i < 8; i++)
    {
        const int to_sq = get_move_to_ind(square, KING_MOVES[i][0], KING_MOVES[i][1]);

        if (to_sq == -1)
            continue;

        if (is_bit_set(board.bitboards[att_p_bb],to_sq))
            return true;
    }
    return false;
}