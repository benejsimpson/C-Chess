#include "core/bitboard.hpp"
#include <iostream>

                                                                    // debug / display helpers

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

