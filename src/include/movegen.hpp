#pragma once
#include <vector>
#include "board.hpp"
#include "move.hpp"

std::vector<Move> generate_pseudo_legal_moves(const Board &board);
std::vector<Move> generate_legal_moves(const Board &board);
std::vector<Move> generate_legal_moves_for_square(const Board& board, int square);

bool same_move(const Move& a, const Move& b);
bool is_in_check(const Board& board, bool white_king);