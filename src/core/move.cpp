#include "include/move.hpp"
#include "include/board.hpp"

// define struct Move

// implement make_move
void apply_move(Board &board, const Move &move)
{
    // move piece
    board.squares[move.to] = move.piece;

    // clear original square
    board.squares[move.from] = Empty;

    // (captured piece is overwritten automatically)

    // switch turn
    board.white_to_move = !board.white_to_move;
}
// implement undo_move