#pragma once
#include "board.hpp"
#include "move.hpp"
#include "core/movelist.hpp"
#include <vector>
#include <array>

const int KNIGHT_MOVES[8][2] = {{1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1}};
const int KING_MOVES[8][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
const int DIAGONAL_MOVES[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
const int STRAIGHT_MOVES[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

constexpr std::array<BitB, 64> build_pawn_attacks(bool white);
constexpr std::array<BitB, 64> build_knight_attacks();
constexpr std::array<BitB, 64> build_king_attacks();

inline constexpr std::array<BitB, 64> WHITE_PAWN_ATTACKS = build_pawn_attacks(true);
inline constexpr std::array<BitB, 64> BLACK_PAWN_ATTACKS = build_pawn_attacks(false);
inline constexpr std::array<BitB, 64> KNIGHT_ATTACKS = build_knight_attacks();
inline constexpr std::array<BitB, 64> KING_ATTACKS = build_king_attacks();

// generates all possible moves that can be made by a side
MoveList generate_pseudo_legal_moves(const Board &board);

// generates only legal moves that can be made - not in check after
MoveList generate_legal_moves(const Board &board);

// legal move generation for a specific piece on a square
MoveList generate_legal_moves_for_square(const Board& board, int square);

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
constexpr int get_move_to_ind(int from, int d_file, int d_rank)
{
const     int to_rank = index_to_rank(from) + d_rank;
const     int to_file = index_to_file(from) + d_file;
    
    if (!is_valid_file_rank(to_file,to_rank))
        return -1;
    
    return from + (8 * d_rank) + d_file;
}
inline bool is_checkmate(Board &board);


                                                                        // attack masks
constexpr BitB pawn_attack_mask(const int square, const bool white)
{
    BitB res = EMPTY_BB;
    const int d_rank = white ? 1 : -1;
    const int d_files[2] = {-1, 1};
    for (int d_file : d_files)
    {
        const int att_sq = get_move_to_ind(square, d_file, d_rank);
        
        if (att_sq == -1)
            continue;
        res |= square_mask(att_sq);
    }
    return res;
}

constexpr BitB knight_attack_mask(int square)
{
    BitB res = EMPTY_BB;
    for (int i = 0; i < 8; i++)
    {
        const int d_file = KNIGHT_MOVES[i][0];
        const int d_rank = KNIGHT_MOVES[i][1];
        const int att_sq = get_move_to_ind(square, d_file, d_rank);
        if (att_sq == -1)
            continue;
        res |= square_mask(att_sq);
    }
    return res;
}

constexpr BitB king_attack_mask(int square)
{
    BitB res = EMPTY_BB;
    for (int i = 0; i < 8; i++)
    {
        const int d_file = KING_MOVES[i][0];
        const int d_rank = KING_MOVES[i][1];
        const int att_sq = get_move_to_ind(square, d_file, d_rank);
        if (att_sq == -1)
            continue;
        res |= square_mask(att_sq);
    }
    return res;
}
                                                                        // Load attack masks

constexpr std::array<BitB, 64> build_pawn_attacks(bool white)
{
    std::array<BitB, 64> table{};
    for (int sq = 0; sq < 64;sq++)
    {
        if (sq < 8 || sq >= 56)
        {
            table[sq] = EMPTY_BB;
            continue;
        }
        table[sq] = pawn_attack_mask(sq, white);
    }
    return table;
}

constexpr std::array<BitB,64> build_knight_attacks()
{
    std::array<BitB, 64> table{};
    for (int sq = 0; sq < 64;sq++)
    {
        table[sq] = knight_attack_mask(sq);
    }
    return table;
}

constexpr std::array<BitB, 64> build_king_attacks()
{
    std::array<BitB, 64> table{};
    for (int sq = 0; sq < 64;sq++)
    {
        table[sq] = king_attack_mask(sq);
    }
    return table;
}

constexpr BitB diagonal_attack_mask(int sq);
constexpr BitB straight_attack_mask(int sq);
