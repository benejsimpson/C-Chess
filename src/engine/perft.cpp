// src/engine/perft.cpp
#include "engine/perft.hpp"
#include "core/movegen.hpp"
#include "core/makemove.hpp"
#include "core/movelist.hpp"

using namespace std;

uint64_t perft(const Board &board, int depth)
{
    if (depth == 0)
        return 1;

    MoveList moves = generateLegalMoves(board); // generates all legal moves in the position

    uint64_t nodes = 0;
    int en_passant = 0;
    int captures = 0;
    int castles = 0;
    int promotions = 0;
    int checks = 0;

    for (const Move &move : moves) // loop through each possible next move
    {
        const string from =
            squareToName(moveFrom(move));

        const string to =
            squareToName(moveTo(move));

        const char piece =
            pieceToChar(board.squares[moveFrom(move)]);

        const char capturedPiece =
            pieceToChar(board.squares[moveTo(move)]);

        const MoveFlag flag =
            static_cast<MoveFlag>(moveFlag(move));

        Board copy = board;    // create a copy of the board starting position
        applyMove(copy, move); // applies the next move to the copy of the board

        if (capturedPiece == 'k' || capturedPiece == 'K')
        {
            cout << "ERROR: attempted to capture king in perft\n";
            continue;
        }

        if (isInCheck(copy, !copy.whiteToMove)) // if the move leaves the king in check, skip this move
        {
            cout << "ERROR: Move leaves king in check\n";
            continue;
        }

        if (flag == EN_PASSANT)
            en_passant++;

        if (flag == CAPTURE)
            captures++;

        if (flag == KING_CASTLE || flag == QUEEN_CASTLE)
            castles++;

        if (isPromotionFlag(flag))
            promotions++;

        nodes += perft(copy, depth - 1); // recursively looks for moves until depth has been reached
    }

    cout << "Depth: " << depth << " | Nodes: " << nodes
         << " | En Passant: " << en_passant
         << " | Captures: " << captures
         << " | Castles: " << castles
         << " | Promotions: " << promotions
         << '\n';
    return nodes;
}
