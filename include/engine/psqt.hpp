#pragma once

#include "core/board.hpp"
#include "core/move.hpp"
#include "core/movelist.hpp"
#include "engine/constants.hpp"

// returns bitboard of [white/black] pieces on the board
// includes N B R Q but not pawns or king
inline const BitB piecesOnBoard(const Board &board, const bool white)
{
    if (white)
    {
        return board.pieceBBs[WHITE_BB_INDS[1]] | // N
               board.pieceBBs[WHITE_BB_INDS[2]] | // B
               board.pieceBBs[WHITE_BB_INDS[3]] | // R
               board.pieceBBs[WHITE_BB_INDS[4]];  // Q
    }
    else
    {
        return board.pieceBBs[BLACK_BB_INDS[1]] | // N
               board.pieceBBs[BLACK_BB_INDS[2]] | // B
               board.pieceBBs[BLACK_BB_INDS[3]] | // R
               board.pieceBBs[BLACK_BB_INDS[4]];  // Q
    }
}

enum PSQT_Phase
{
    OPENING_PAWNS,
    OPENING_PIECES,
    OPENING_KING,

    MIDDLE_PAWNS,
    MIDDLE_PIECES,
    MIDDLE_KING,

    ENG_PAWNS,
    END_PIECES,
    END_KING,

    CASTLE_KINGSIDE,
    CASTLE_QUEENSIDE,
};

// returns number of pieces on the board
// includes N B R Q but not pawns or king
inline const int countPiecesOnBoard(const Board &board)
{
    return countBits(piecesOnBoard(board, true) | piecesOnBoard(board, false));
}

// returns the PSQT for all PAWNS for each phase of the game and castling position
inline PSQT pawnPSQTForPhase(const Board &board, bool white)
{
    const BitB whitePawns = board.pieceBBs[pieceToBitboardIndex(WP)];
    const BitB blackPawns = board.pieceBBs[pieceToBitboardIndex(BP)];

    if (countPiecesOnBoard(board) < 4)
        return ENDGAME_PAWNS_PSQT;

    if (white)
    {
        if (board.whiteHasKsCastled)
        {
            return CASTLED_PAWNS_PSQT[0];
        }
        else if (board.whiteHasQsCastled)
        {
            return CASTLED_PAWNS_PSQT[1];
        }
    }
    else
    {
        if (board.blackHasKsCastled)
        {
            return CASTLED_PAWNS_PSQT[0];
        }
        else if (board.blackHasQsCastled)
        {
            return CASTLED_PAWNS_PSQT[1];
        }
    }
    return OPENING_PAWNS_PSQT;
}

// returns the PSQT for all PIECES (N, B, R, Q) for each phase of the game
inline PSQT_Set piecePSQTForPhase(const Board &board)
{
    const int numPieces = countPiecesOnBoard(board);

    if (numPieces >= 11) // opening pieces psqt
    {
        return OPENING_PIECES_PSQT;
    }

    if (numPieces >= 4 && numPieces <= 10) // middle game pieces psqt
    {
        return MIDDLEGAME_PIECES_PSQT;
    }

    else // end game pieces psqt
    {
        return ENDGAME_PIECES_PSQT;
    }
}

// returns the PSQT for KING for each phase of the game and castling position
inline PSQT kingPSQTForPhase(const Board &board, bool white)
{
    const int numPieces = countPiecesOnBoard(board);

    if (numPieces < 4) // end game king psqt
    {
        return ENDGAME_KING_PSQT;
    }

    // king castled psqt
    if (white)
    {
        if (board.whiteHasKsCastled)
        {
            return CASTLED_KING_PSQT[0];
        }
        else if (board.whiteHasQsCastled)
        {
            return CASTLED_KING_PSQT[1];
        }
    }
    else
    {
        if (board.blackHasKsCastled)
        {
            return CASTLED_KING_PSQT[0];
        }
        else if (board.blackHasQsCastled)
        {
            return CASTLED_KING_PSQT[1];
        }
    }
    return OPENING_KING_PSQT;
}
