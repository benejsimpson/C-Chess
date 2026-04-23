#include "core/move.hpp"
#include "core/board.hpp"

void apply_move(Board &board, const Move &move)
{
    // Promotion is special because the pawn changes into a new piece
    if (is_promotion(move) && move.promotion != Empty)
    {
        // Remove pawn from start square
        remove_piece(board, move.from, move.piece);

        // Remove captured piece if there is one
        if (!is_empty_p(move.captured))
        {
            remove_piece(board, move.to, move.captured);
        }

        // Place promoted piece
        place_piece(board, move.to, move.promotion);
    }
    else
    {
        // Normal move or normal capture
        move_piece(board, move.from, move.to);
    }

    // Change whose turn it is
    board.white_to_move = !board.white_to_move;
}