#pragma once
#include "utils.h"

struct Board;

// move representation
struct Move;

// apply move to board
void make_move(Board& board, const Move& move);

// undo move (optional later)
void undo_move(Board& board, const Move& move);


