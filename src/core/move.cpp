#include "core/move.hpp"
#include "core/board.hpp"

void apply_move(Board &board, const Move &move)
{
    Piece moved_piece = move.piece;
    if (is_promotion(move) && move.promotion != Empty)
    {
        moved_piece = move.promotion;
    }

    board.squares[move.from] = Empty;
    board.squares[move.to] = moved_piece;
    board.white_to_move = !board.white_to_move;
}
