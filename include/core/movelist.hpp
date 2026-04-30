// core/movelist.hpp
#pragma once
#include <iostream>
#include "core/move.hpp"

constexpr int MAX_MOVES = 218;

struct MoveList
{
    Move moves[MAX_MOVES];
    int count = 0;

    void add(Move move)
    {
        if (count >= MAX_MOVES)
        {
            std::cout << "MOVELIST OVERFLOW\n";
            return;
        }
        moves[count++] = move;
    }

    constexpr inline int get_move_score(const Move &move)
    {
        
    }

    void clear()
    {
        count = 0;
    }

    bool empty() const
    {
        return count == 0;
    }

    Move front() const
    {
        return moves[0];
    }

    Move& operator[](int index)
    {
        return moves[index];
    }

    const Move& operator[](int index) const
    {
        return moves[index];
    }

    Move* begin()
    {
        return moves;
    }

    Move* end()
    {
        return moves + count;
    }

    const Move* begin() const
    {
        return moves;
    }

    const Move* end() const
    {
        return moves + count;
    }
};