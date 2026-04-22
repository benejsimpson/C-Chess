#pragma once
#include <vector>
#include "src\include\board.hpp"
#include "src\include\move.hpp"

std::vector<Move> generate_pseudo_legal_moves(const Board& board);
std::vector<Move> generate_legal_moves(const Board& board);

bool is_in_check(const Board& board);

static void generate_pawn_moves(const Board &board, vector<Move> &moves, int square);