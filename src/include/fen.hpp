#pragma once
#include "utils.h"

/*
    Parse FEN & load position
*/

struct Board;

// load FEN into board
std::string reverse_fen(std::string fen);
void load_fen(Board &board, const std::string &fen);

// export board to FEN
std::string export_fen(const Board& board);

