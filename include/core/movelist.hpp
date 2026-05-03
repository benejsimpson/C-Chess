// core/movelist.hpp
#pragma once
#include <iostream>
#include "core/move.hpp"
#include "core/movegen.hpp"
#include "core/piece.hpp"
#include "engine/move_scoring.hpp"

#include <algorithm>

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

    void clear()
    {   count = 0;}

    bool empty() const
    {   return count == 0;}

    Move front() const
    {   return moves[0];}

    Move &operator[](int index)
    {   return moves[index];}

    const Move &operator[](int index) const
    {   return moves[index];}

    Move *begin()
    {   return moves;}

    Move *end()
    {   return moves + count;}

    const Move *begin() const
    {   return moves;}

    const Move *end() const
    {   return moves + count;}
};



// applies move scoring function to order moves so moves that are likely to be good are searched first
inline void orderMoves(MoveList &m, const Board &board)
{
    ScoredMove scoredMoves[MAX_MOVES];

    for (int i = 0; i < m.count; ++i)
    {
        scoredMoves[i].move = m.moves[i];
        scoredMoves[i].score = getMoveScore(m.moves[i], board); 
    }

    std::sort(
        scoredMoves,
        scoredMoves + m.count,
        [](const ScoredMove &a, const ScoredMove &b)
        {
            return a.score > b.score;
        }
    );

    for (int i = 0; i < m.count; ++i)
    {
        m.moves[i] = scoredMoves[i].move;
    }
}