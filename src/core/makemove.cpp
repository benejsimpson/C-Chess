#include "core/makemove.hpp"
#include <iostream>

                                                            // Internal helpers

static void remove_castling_rights_for_rook(Board& board, int square, Piece rook)
{
    // White rooks
    if (rook == WR)
    {
        if (square == 0)
            board.white_queen_side = false;
        else if (square == 7)
            board.white_king_side = false;
    }

    // Black rooks
    if (rook == BR)
    {
        if (square == 56)
            board.black_queen_side = false;
        else if (square == 63)
            board.black_king_side = false;
    }
}

static void remove_castling_rights_for_king(Board& board, Piece king)
{
    if (king == WK)
    {
        board.white_king_side = false;
        board.white_queen_side = false;
    }
    else if (king == BK)
    {
        board.black_king_side = false;
        board.black_queen_side = false;
    }
}

static bool is_capture(const Board &board, Move move) // UNUSED
{
    return is_bit_set(all_occupancy(board), move_to(move)) ||
           move_flag(move) == EN_PASSANT; // need to check en passant move flag as no piece on square
}

                                                                    // Main move application

void apply_move(Board& board, Move move)
{
    const int from = move_from(move);
    const int to = move_to(move);
    const MoveFlag flag = static_cast<MoveFlag>(move_flag(move));

    const Piece moved_piece = board.squares[from];
    const Piece captured_piece = board.squares[to];

    if (captured_piece == WK || captured_piece == BK)
    {
        std::cout << "ERROR: attempted to capture king: "
                  << from << " -> " << to << '\n';
        return;
    }

    // move removes possible en-passant
    // allow en-passant only when double pawn move
    board.en_passant_square = -1;

    // Update castling rights before moving pieces
    // if king moves, that side loses both castling rights
    if (get_piece_type(moved_piece) == KING)
    {
        remove_castling_rights_for_king(board, moved_piece);
    }

    // if a rook moves from its original square, that side loses that rook's castling right
    if (get_piece_type(moved_piece) == ROOK)
    {
        remove_castling_rights_for_rook(board, from, moved_piece);
    }

    // if a rook is captured on its original square, that side loses that castling right
    if (captured_piece != Empty && get_piece_type(captured_piece) == ROOK)
    {
        remove_castling_rights_for_rook(board, to, captured_piece);
    }

    // CAPTURE
    if (captured_piece != Empty)
        remove_piece(board, to);

                                                                    // Handle move by flag

    // en passant capture
    if (flag == EN_PASSANT)
    {
        move_piece(board, from, to);
        // captured pawn is not on square moved to
        // adjust index to remove captured pawn
        remove_piece(board, to + (board.white_to_move ? -8 : 8));
    }

    // king-side castle
    else if (flag == KING_CASTLE)
    {
        move_piece(board, from, to);
        // move rook as well
        // white : 7 -> 5, black : 63 -> 61
        move_piece(board,
            (board.white_to_move ? 7 : 63),
            (board.white_to_move ? 5 : 61));
    }

    // queen-side castle
    else if (flag == QUEEN_CASTLE)
    {
        move_piece(board, from, to);
        // move rook as well
        // white : 0 -> 3, black : 56 -> 59
        move_piece(board,
            (board.white_to_move ? 0 : 56),
            (board.white_to_move ? 3 : 59));
    }

    // promotion
    else if (is_promotion_flag(flag))
    {
        remove_piece(board, from);
        // place promoted piece in place of piece
        place_piece(board, to, promotion_piece_from_flag(flag, board.white_to_move));
    }

    // normal move / double pawn move
    else
    {
        move_piece(board, from, to);

        // If a pawn moved 2 squares, record the en passant target square
        if (flag == DOUBLE_PAWN)
            board.en_passant_square = from + (board.white_to_move ? 8 : -8);
    }

    // change side to move
    board.white_to_move = !board.white_to_move;


    // increment fullmove_number after black moves
    if (board.white_to_move)
    {
        board.fullmove_number++;
    }
}

