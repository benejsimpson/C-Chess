// core/movelist.cpp

#include "core/movelist.hpp"
#include "core/makemove.hpp"

inline int getMoveScore(const Move &move, const Board &board)
{
    int score = 0;
    const int flag = moveFlag(move);

    // Checks
    Board copy = board;
    Undo undo;
    makeMove(copy, move, undo);
    if (isInCheck(copy,!board.whiteToMove))
        score += 10000;

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