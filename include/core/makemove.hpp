#pragma once
#include "board.hpp"
#include "move.hpp"
#include "engine/zobrist.hpp"

// Apply one move to the board
// updates:
// - squares
// - bitboards
// - side to move
// - en passant square
// - castling rights
// - fullmove number
void applyMove(Board &board, Move move);