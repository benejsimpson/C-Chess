#pragma once
#include "board.hpp"
#include "move.hpp"

// Apply one move to the board.
// This updates:
// - squares
// - bitboards
// - side to move
// - en passant square
// - castling rights
// - fullmove number
void apply_move(Board& board, const Move& move);