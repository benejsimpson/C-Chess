#pragma once

#include "core/board.hpp"
#include "core/move.hpp"
#include <array>
#include <cstdint>

using Hash = uint64_t;

struct Zobrist
{
    std::array<std::array<Hash, 64>, 12> piece_square_hashes;   // [piece][square]

    // 16 possible castling rights combinations (KQkq) : 0000 to 1111
    std::array<Hash, 16> castling_hashes{};                     // [castling_rights]

    // 8 possible en passant files (a-h) : 0000 to 1111
    std::array<Hash, 8> en_passant_file_hashes{};               // [file]
    
    Hash white_to_move_hash = 0;

};

extern Zobrist ZOBRIST;

void init_zobrist();

Hash generate_hash(const Board &board);

void update_hash_for_moved_piece(Board &board, const Move &move);
void update_hash_for_castling(Board &board, const Move &move);
void update_hash_for_en_passant(Board &board, int old_ep_square, int new_ep_square);
int count_repetitions(const std::vector<Hash> &history, Hash current_hash);

// returns true if the current position is drawn by repetition
// uses hashes stored in board.position_history to count number of times current position has occurred in the game
inline bool is_draw_by_repetition(const Board &board)
{
    return count_repetitions(board.position_history, board.hash) >= 3;
}
