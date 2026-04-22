#pragma once
#include "utils.h"
#include "move.hpp"
#include "board.hpp"
#include "movegen.hpp"

struct Board;

// initialise window
void init_window();

// run main loop
void run_ui(Board& board);

std::vector<Move> generate_legal_moves_for_square(const Board& board, int square);