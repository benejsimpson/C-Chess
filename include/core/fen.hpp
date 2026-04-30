#pragma once
#include "utils.h"

inline constexpr char START_FEN[58] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";

// Parse FEN & load position
struct Board;

// load FEN into board
void load_fen(Board &board, const std::string &fen);

// export board to FEN
std::string export_fen(const Board& board);

