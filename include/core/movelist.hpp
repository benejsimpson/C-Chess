// core/movelist.hpp


#pragma once
#include "core/move.hpp"

constexpr int MAX_MOVES = 218;

/*
    Moves stored as fixed size array instead of vector
    max moves in any position found to be 218 so use this as size
*/

// array of up to 218 moves
// made with create_move(from, to, flag)
// Stored as 16 bit : FFFF TTTTTT SSSSSS
// F = (MoveFlag) flag , T = target index S = start index
// clear after use
struct MoveList
{
    Move moves[MAX_MOVES];
    int count = 0;

    inline void add(Move move)
    {
        moves[count++] = move;
    }
    inline void clear()
    {
        count = 0;
    }
    inline bool has_moves()
    {
        return count != 0;
    }
    
    // enable range-based for
    Move* begin() { return moves; }
    Move* end()   { return moves + count; }

    const Move* begin() const { return moves; }
    const Move* end()   const { return moves + count; }
};
