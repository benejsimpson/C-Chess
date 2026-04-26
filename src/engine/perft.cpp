// src/engine/perft.cpp
#include "engine/perft.hpp"
#include "core/movegen.hpp"
#include "core/makemove.hpp"

using namespace std;

uint64_t perft(const Board& board, int depth)
{
    if (depth == 0)
        return 1;

    vector<Move> moves = generate_legal_moves(board); // generates all legal moves in the position

    uint64_t nodes = 0;

    for (const Move& move : moves) // loop through each possible next move
    {
        Board copy = board; // create a copy of the board starting position
        apply_move(copy, move); // applies the next move to the copy of the board
        nodes += perft(copy, depth - 1); // recursively looks for moves until depth has been reached
    }

    return nodes;
}
