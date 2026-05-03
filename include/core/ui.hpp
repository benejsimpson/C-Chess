#pragma once
#include "utils.hpp"
#include "move.hpp"
#include "board.hpp"
#include "movegen.hpp"

struct Board;

// initialise window
void init_window();

// run main loop
void run_ui(Board &board);

MoveList generateLegalMovesForSquare(const Board &board, int square);