// core/movelist.hpp
#pragma once
#include <iostream>
#include "core/move.hpp"
#include "core/movegen.hpp"
#include "core/piece.hpp"

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

struct ScoredMove
{
    Move move;
    int score;
};

inline int getMoveScore(const Move &move, const Board &board)
{
    int score = 0;
    const int flag = moveFlag(move);

    // Castling
    if (flag == KING_CASTLE || flag == QUEEN_CASTLE)
        score += 100;

    // Promotions
    if (flag == Q_PROMO) score += 9000;
    else if (flag == R_PROMO) score += 5000;
    else if (flag == B_PROMO) score += 3000;
    else if (flag == N_PROMO) score += 3000;

    // Captures
    if (isCapture(board, move))
    {
        const int from = moveFrom(move);
        const int to = moveTo(move);

        const Piece movedPiece = board.squares[from];
        Piece capturedPiece = board.squares[to];

        // En passant
        if (flag == EN_PASSANT)
            capturedPiece = board.whiteToMove ? BP : WP;

        const int movedInd = pieceToBitboardIndex(movedPiece) % 6;
        const int capturedInd = pieceToBitboardIndex(capturedPiece) % 6;

        const int movedValue = PIECE_MATERIAL_SCORE[movedInd];
        const int capturedValue = PIECE_MATERIAL_SCORE[capturedInd];

        // MVV-LVA
        score += 10000;
        score += capturedValue * 10;
        score -= movedValue;

        // Slight penalty if the capture square is defended
        const BitB defenders = board.whiteToMove
            ? board.blackAttacksMask
            : board.whiteAttacksMask;

        if (isBitSet(defenders, to))
            score -= movedValue;
    }

    return score;
}

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