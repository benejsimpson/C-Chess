#include "core/makemove.hpp"

// --------------------------------------------------
// Internal helpers
// --------------------------------------------------

static bool is_rook_start_square(int square, Piece piece)
{
    if (piece == WR && (square == 0 || square == 7))
        return true;

    if (piece == BR && (square == 56 || square == 63))
        return true;

    return false;
}

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

// --------------------------------------------------
// Main move application
// --------------------------------------------------

void apply_move(Board& board, const Move& move)
{
    Piece moved_piece = move.piece;
    Piece captured_piece = move.captured;

    // move removes possible en-passant
    // allow en-passant only when double pawn move
    board.en_passant_square = -1;

    // ------------------------------------------
    // Update castling rights before moving pieces
    // ------------------------------------------

    // if king moves, that side loses both castling rights
    if (get_piece_type(moved_piece) == KING)
    {
        remove_castling_rights_for_king(board, moved_piece);
    }

    // if a rook moves from its original square, that side loses that rook's castling right
    if (get_piece_type(moved_piece) == ROOK)
    {
        remove_castling_rights_for_rook(board, move.from, moved_piece);
    }

    // if a rook is captured on its original square, that side loses that castling right
    if (captured_piece != Empty && get_piece_type(captured_piece) == ROOK)
    {
        remove_castling_rights_for_rook(board, move.to, captured_piece);
    }

    // ------------------------------------------
    // Handle move by flag
    // ------------------------------------------

    // en passant capture
    if (move.flag == EN_PASSANT)
    {
        move_piece(board, move.from, move.to);

        // Captured pawn is not on the target square.
        int captured_square;

        if (is_white(moved_piece))
            captured_square = move.to - 8;
        else
            captured_square = move.to + 8;

        remove_piece(board, captured_square, captured_piece);
    }

    // king-side castle
    else if (move.flag == KING_CASTLE)
    {
        move_piece(board, move.from, move.to);

        // Move rook as well
        if (moved_piece == WK)
        {
            move_piece(board, 7, 5);   // h1 -> f1
        }
        else if (moved_piece == BK)
        {
            move_piece(board, 63, 61); // h8 -> f8
        }
    }

    // queen-side castle
    else if (move.flag == QUEEN_CASTLE)
    {
        move_piece(board, move.from, move.to);

        // Move rook as well
        if (moved_piece == WK)
        {
            move_piece(board, 0, 3);   // a1 -> d1
        }
        else if (moved_piece == BK)
        {
            move_piece(board, 56, 59); // a8 -> d8
        }
    }

    // promotion without capture
    else if (move.flag == PROMOTION)
    {
        remove_piece(board, move.from, moved_piece);
        place_piece(board, move.to, move.promotion);
    }

    // promotion with capture
    else if (move.flag == PROMO_CAPTURE)
    {
        remove_piece(board, move.from, moved_piece);
        remove_piece(board, move.to, captured_piece);
        place_piece(board, move.to, move.promotion);
    }

    // normal move / normal capture / double pawn move
    else
    {
        if (captured_piece != Empty)
        {
            remove_piece(board, move.to, captured_piece);
        }

        move_piece(board, move.from, move.to);

        // If a pawn moved 2 squares, record the en passant target square.
        if (move.flag == DOUBLE_PAWN)
        {
            if (is_white(moved_piece))
                board.en_passant_square = move.from + 8;
            else
                board.en_passant_square = move.from - 8;
        }
    }

    // change side to move
    board.white_to_move = !board.white_to_move;


    // increment fullmove_number after black moves
    if (board.white_to_move)
    {
        board.fullmove_number++;
    }
}
