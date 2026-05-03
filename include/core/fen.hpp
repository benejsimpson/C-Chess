#pragma once
#include "utils.hpp"
#include "core/constants.hpp"

// Parse FEN & load position
struct Board;

// load FEN into board
void loadFEN(Board &board, const std::string &fen);

// export board to FEN
std::string exportFEN(const Board &board);
