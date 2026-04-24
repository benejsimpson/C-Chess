#pragma once
#include "board.hpp"
#include "move.hpp"
#include <vector>

// generates all possible moves that can be made by a side
std::vector<Move> generate_pseudo_legal_moves(const Board &board);
// generates only legal moves that can be made - not in check after
std::vector<Move> generate_legal_moves(const Board &board);
// legal move generation for a specific piece on a square
std::vector<Move> generate_legal_moves_for_square(const Board& board, int square);

bool same_move(const Move& a, const Move& b);
bool is_in_check(const Board& board, bool white_king);
bool king_can_castle_kingside(const Board &board, bool white);
bool king_can_castle_queenside(const Board &board, bool white);