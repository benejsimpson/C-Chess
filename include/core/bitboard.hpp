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

constexpr BitB square_mask(int square);
constexpr bool is_bit_set(BitB bb, int square);
constexpr void set_bit(BitB& bb, int square);
constexpr void clear_bit(BitB& bb, int square);

                                                                    // Counting / scanning
inline int count_bits(BitB bb);
inline int lsb_index(BitB bb);
inline int pop_lsb(BitB& bb);

                                                                    // Board masks
constexpr BitB rank_mask(int rank);
constexpr BitB file_mask(int file);


                                                                    // debug / display helpers
void print_BitB(BitB bb);