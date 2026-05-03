#pragma once

#include "core/constants.hpp"

#include <cstdint>
using BitB = uint64_t;
//--------------------- Constants

// 00000000
// 00000000
// 00000000
// 00000000
// 00000000
// 00000000
// 00000000
// 00000000
constexpr BitB EMPTY_BB = 0ULL;
// 11111111
// 11111111
// 11111111
// 11111111
// 11111111
// 11111111
// 11111111
// 11111111
constexpr BitB FULL_BB = ~0ULL;
// 10000000
// 10000000
// 10000000
// 10000000
// 10000000
// 10000000
// 10000000
// 10000000
constexpr BitB FILE_A = 0x0101010101010101ULL;

//--------------------- Square Helpers

// returns a 0ULL with single bit set at square index
constexpr BitB square_mask(int square)
{
    return (1ULL << square);
}

// returns true if bit at square index is set
inline const bool isBitSet(BitB bb, int square)
{
    return (square_mask(square) & bb) != 0;
}

// sets the bit at square index
// mutates bitboard
constexpr void set_bit(BitB &bb, int square)
{
    bb |= square_mask(square);
}

// unsets the bit at square index
// mutates bitboard
constexpr void clear_bit(BitB &bb, int square)
{
    bb &= ~square_mask(square);
}

//--------------------- Counting / Scanning

// counts the number of set bits in ULL
inline const int countBits(BitB bb)
{
    return __builtin_popcountll(bb);
}

// returns the index of the least significant bit
// for King bb this would be the index of the king
inline const int LsbIndex(BitB bb)
{
    if (bb == 0)
    {
        std::cout << "LsbIndex returned -1\n";
        return -1;
    }

    return __builtin_ctzll(bb); // counts trailling zeros of ULL
}

// removes the least significant bit and returns its index
inline int popLSB(BitB &bb)
{
    int lsb_ind = LsbIndex(bb); // gets index of least significant bit
    clear_bit(bb, lsb_ind);     // unsets bit at index
    return lsb_ind;             // returns the index
}

//--------------------- Board Masks

// returns ULL with set bits on rank indexed 0-7
inline constexpr BitB rankMask(const int rank)
{
    return 0xFFULL << (8 * rank);
}

// returns ULL with set bits on file indexed 0-7
inline constexpr BitB fileMask(int file)
{
    return FILE_A << file;
}

//--------------------- Debug / Display Helpers

// prints 8x8 chess board representation of bitboard
// from white pov (a1 bottom left, h8 top right)
void print_BitB(BitB bb)
{
    for (int rank = 7; rank >= 0; --rank)
    {
        BitB row = (bb >> (rank * 8)) & 0xFFULL;

        std::cout << row << '\n';
    }
}