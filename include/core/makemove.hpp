#pragma once
#include "board.hpp"
#include "move.hpp"
#include "engine/zobrist.hpp"

//--------------------- Undo Move
/*
    Stores board state data that cannot be obtained from a Move

*/

struct Undo
{
    Piece capturedPiece;

    int enPassantSquare;

    bool whiteCanKsCastle;
    bool whiteCanQsCastle;
    bool blackCanKsCastle;
    bool blackCanQsCastle;

    int fullmoveNumber;

    Hash hash;
};

// Apply one move to the board
// updates:
// - squares
// - bitboards
// - side to move
// - en passant square
// - castling rights
// - fullmove number
void makeMove(Board &board, Move move, Undo &undo);

void updateUndoMove(Board &board, Move move, Undo &undo);

void undoMove(Board &board, Move move, const Undo &undo);