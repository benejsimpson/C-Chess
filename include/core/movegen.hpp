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

static inline std::vector<int> squares_to_check_between_king_and_rook(bool white, bool kingside);
static bool squares_between_king_and_rook_clear(const Board &board, bool white, bool kingside);

// 1. index moving from
// 2. change in file
// 3. change in rank
// -> index moved to : -1 if not valid
static inline int get_move_to_ind(int from, int d_file, int d_rank)
{
    int to_rank = index_to_rank(from) + d_rank;
    int to_file = index_to_file(from) + d_file;
    
    if (!is_valid_file_rank(to_file,to_rank))
        return -1;
    
    return from + (8 * d_rank) + d_file;
}
inline bool is_checkmate(Board &board);