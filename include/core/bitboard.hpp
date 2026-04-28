#pragma once
#include <cstdint>
using BitB = uint64_t;
                                                                    // Constants

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
constexpr BitB FULL_BB  = ~0ULL;
// 10000000
// 10000000
// 10000000
// 10000000
// 10000000
// 10000000
// 10000000
// 10000000
constexpr BitB FILE_A = 0x0101010101010101ULL;

                                                                    // basic square masks


                                                                    // basic square masks

// returns a 0ULL with single bit set at square index
constexpr BitB square_mask(int square)
{
    return (1ULL << square);
}
    
// returns true if bit at square index is set
inline const bool is_bit_set(BitB bb, int square)
{
    return (square_mask(square) & bb) != 0;
}

// sets the bit at square index
// mutates bitboard
constexpr void set_bit(BitB& bb, int square)
{
    bb |= square_mask(square);
}

// unsets the bit at square index
// mutates bitboard
constexpr void clear_bit(BitB& bb, int square)
{
    bb &= ~square_mask(square);
}

                                                                    // Counting / scanning

// counts the number of set bits in ULL
inline const int count_bits(BitB bb)
{
    return __builtin_popcountll(bb); 
}

// returns the index of the least significant bit
// for King bb this would be the index of the king
inline const int lsb_index(BitB bb)
{
    if (bb == 0)
    {
        std::cout << "lsb_index returned -1\n";
        return -1;
    }
    
    return __builtin_ctzll(bb); // counts trailling zeros of ULL
}

// removes the least significant bit and returns its index
inline int pop_lsb(BitB& bb)
{
    int lsb_ind = lsb_index(bb); // gets index of least significant bit
    clear_bit(bb,lsb_ind); // unsets bit at index
    return lsb_ind; // returns the index
}


                                                                    // Board masks

// returns ULL with set bits on rank indexed 0-7
constexpr BitB rank_mask(const int rank)
{
    return 0xFFULL << (8 * rank);
}

// returns ULL with set bits on file indexed 0-7
constexpr BitB file_mask(int file)
{
    return FILE_A << file;
}



                                                                    // debug / display helpers
void print_BitB(BitB bb);