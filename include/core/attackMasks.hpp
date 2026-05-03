#pragma once

#include "core/bitboard.hpp"
#include "core/utils.hpp"
#include "core/constants.hpp"

#include <array>
#include <cstdint>

        // attack masks for tables
inline constexpr BitB pawnAttackMask(const int square, const bool white)
{
    BitB res = EMPTY_BB;
    const int d_rank = white ? 1 : -1;
    const int d_files[2] = {-1, 1};
    for (int d_file : d_files)
    {
        const int att_sq = getNextMoveIndex(square, d_file, d_rank);

        if (att_sq == -1)
            continue;
        res |= square_mask(att_sq);
    }
    return res;
}

constexpr BitB knightAttackMask(int square)
{
    BitB res = EMPTY_BB;
    for (int i = 0; i < 8; i++)
    {
        const int d_file = KNIGHT_MOVES[i][0];
        const int d_rank = KNIGHT_MOVES[i][1];
        const int att_sq = getNextMoveIndex(square, d_file, d_rank);
        if (att_sq == -1)
            continue;
        res |= square_mask(att_sq);
    }
    return res;
}

constexpr BitB kingAttackMask(int square)
{
    BitB res = EMPTY_BB;
    for (int i = 0; i < 8; i++)
    {
        const int d_file = KING_MOVES[i][0];
        const int d_rank = KING_MOVES[i][1];
        const int att_sq = getNextMoveIndex(square, d_file, d_rank);
        if (att_sq == -1)
            continue;
        res |= square_mask(att_sq);
    }
    return res;
}

constexpr std::array<BitB, 64> buildPawnAttacks(bool white)
{
    std::array<BitB, 64> table{};
    for (int sq = 0; sq < 64; sq++)
    {
        table[sq] = pawnAttackMask(sq, white);
    }
    return table;
}

constexpr std::array<BitB, 64> buildKnightAttacks()
{
    std::array<BitB, 64> table{};
    for (int sq = 0; sq < 64; sq++)
    {
        table[sq] = knightAttackMask(sq);
    }
    return table;
}

constexpr std::array<BitB, 64> buildKingAttacks()
{
    std::array<BitB, 64> table{};
    for (int sq = 0; sq < 64; sq++)
    {
        table[sq] = kingAttackMask(sq);
    }
    return table;
}

// BB of all squares attacked by a white pawn on [square]
inline constexpr std::array<BitB, 64> WHITE_PAWN_ATTACKS = buildPawnAttacks(true);
// BB of all squares attacked by a black pawn on [square]
inline constexpr std::array<BitB, 64> BLACK_PAWN_ATTACKS = buildPawnAttacks(false);
// BB of all squares attacked by a knight on [square]
inline constexpr std::array<BitB, 64> KNIGHT_ATTACKS = buildKnightAttacks();
// BB of all squares attacked by a king on [square]
inline constexpr std::array<BitB, 64> KING_ATTACKS = buildKingAttacks();