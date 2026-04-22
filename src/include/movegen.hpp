#pragma once
#include <vector>
#include "board.hpp"
#include "move.hpp"

std::vector<Move> generate_pseudo_legal_moves(const Board& board);
std::vector<Move> generate_legal_moves(const Board& board);

bool is_in_check(const Board& board);