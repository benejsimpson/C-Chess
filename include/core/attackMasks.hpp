#pragma once

#include "core/bitboard.hpp"
#include "core/utils.hpp"
#include "core/constants.hpp"

#include <array>
#include <cstdint>

using AttackMask = std::array<BitB, 64>;

//----------------------------------- attack masks for tables

inline constexpr BitB pawnAttackMask(const int square, const bool white)
{
    BitB res = EMPTY_BB;
    const int dRank = white ? 1 : -1;
    const int d_files[2] = {-1, 1};
    for (int dFile : d_files)
    {
        const int attSq = getNextMoveIndex(square, dFile, dRank);

        if (attSq == -1)
            continue;
        res |= square_mask(attSq);
    }
    return res;
}

constexpr BitB knightAttackMask(int square)
{
    BitB res = EMPTY_BB;
    for (int i = 0; i < 8; i++)
    {
        const int dFile = KNIGHT_MOVES[i][0];
        const int dRank = KNIGHT_MOVES[i][1];
        const int attSq = getNextMoveIndex(square, dFile, dRank);
        if (attSq == -1)
            continue;
        res |= square_mask(attSq);
    }
    return res;
}

constexpr BitB diagonalAttackMask(int square)
{
    BitB res = EMPTY_BB;
    for (int i = 0; i < 4; ++i)
    {
        const int dFile = DIAGONAL_MOVES[i][0];
        const int dRank = DIAGONAL_MOVES[i][1];
        const int attSq = getNextMoveIndex(square, dFile, dRank);
        if (attSq == -1)
            continue;
        res |= square_mask(attSq);
    }
    return res;
}

constexpr BitB straightAttackMask(int square)
{
    BitB res = EMPTY_BB;
    for (int i = 0; i < 4; ++i)
    {
        const int dFile = STRAIGHT_MOVES[i][0];
        const int dRank = STRAIGHT_MOVES[i][1];
        const int attSq = getNextMoveIndex(square, dFile, dRank);
        if (attSq == -1)
            continue;
        res |= square_mask(attSq);
    }
    return res;
}

constexpr BitB kingAttackMask(int square)
{
    BitB res = EMPTY_BB;
    for (int i = 0; i < 8; i++)
    {
        const int dFile = KING_MOVES[i][0];
        const int dRank = KING_MOVES[i][1];
        const int attSq = getNextMoveIndex(square, dFile, dRank);
        if (attSq == -1)
            continue;
        res |= square_mask(attSq);
    }
    return res;
}

constexpr AttackMask buildPawnAttacks(bool white)
{
    AttackMask table{};
    for (int sq = 0; sq < 64; sq++)
    {
        table[sq] = pawnAttackMask(sq, white);
    }
    return table;
}

constexpr AttackMask buildKnightAttacks()
{
    AttackMask table{};
    for (int sq = 0; sq < 64; sq++)
    {
        table[sq] = knightAttackMask(sq);
    }
    return table;
}

constexpr AttackMask buildKingAttacks()
{
    AttackMask table{};
    for (int sq = 0; sq < 64; sq++)
    {
        table[sq] = kingAttackMask(sq);
    }
    return table;
}

constexpr AttackMask buildDiagonalAttacks()
{
    AttackMask table{};
    for (int sq = 0; sq < 64; sq++)
    {
        table[sq] = diagonalAttackMask(sq);
    }
    return table;
}

constexpr AttackMask buildStraightAttacks()
{
    AttackMask table{};
    for (int sq = 0; sq < 64; sq++)
    {
        table[sq] = straightAttackMask(sq);
    }
    return table;
}


// BB of all squares attacked by a white pawn on [square]
inline constexpr AttackMask WHITE_PAWN_ATTACKS = buildPawnAttacks(true);
// BB of all squares attacked by a black pawn on [square]
inline constexpr AttackMask BLACK_PAWN_ATTACKS = buildPawnAttacks(false);
// BB of all squares attacked by a knight on [square]
inline constexpr AttackMask KNIGHT_ATTACKS = buildKnightAttacks();
// BB of all squares attacked by a king on [square]
inline constexpr AttackMask DIAGONAL_ATTACKS = buildDiagonalAttacks();
// BB of all squares attacked by a king on [square]
inline constexpr AttackMask STRAIGHT_ATTACKS = buildStraightAttacks();
// BB of all squares attacked by a king on [square]
inline constexpr AttackMask KING_ATTACKS = buildKingAttacks();
 