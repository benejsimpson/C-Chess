#pragma once
#include "utils.hpp"
#include "core/bitboard.hpp"
#include "core/piece.hpp"

#include <vector>

using BitBoard = uint64_t;

struct Board
{
    Piece squares[64];          // what is on each square of the board
    BitB pieceBBs[12];          // bitboard for each piece and its positions
    BitB whiteAttacksMask = 0;  // bitboard of all squares attacked by white pieces
    BitB blackAttacksMask = 0;  // bitboard of all squares attacked by black pieces

    bool whiteToMove;           // who moves next

    bool whiteCanKsCastle;
    bool whiteCanQsCastle;
    bool blackCanKsCastle;
    bool blackCanQsCastle;

    bool whiteHasKsCastled;
    bool whiteHasQsCastled;
    bool blackHasKsCastled;
    bool blackHasQsCastled;

    int enPassantSquare;        // updated to cell index when en-passant available (-1 if none)

    int fullmoveNumber;

    uint64_t hash = 0;          // Zobrist hash of current board position
    std::vector<uint64_t> positionHistory;
};

//--------------------- Board setup / utility

// Set all squares to empty
// Reset side to move
// Clear castling rights
// Set en-passant square = -1
void clearBoard(Board &board);

// Call clearBoard
// Call loadStartPosition
void resetBoard(Board &board);

//--------------------- BitBoard helpers

inline constexpr bool isPieceOnBoard(const Board &board, Piece piece)
{
    return board.pieceBBs[pieceToBitboardIndex(piece)] != 0;
}

void placePiece(Board &board, int square, Piece piece);
void removePiece(Board &board, int square);
void movePiece(Board &board, int from, int to);

inline BitB whiteOccupancyBB(const Board &board);
inline BitB blackOccupancyBB(const Board &board);
inline BitB allOccupancyBB(const Board &board);
inline BitB diagonalAttackersBB(const Board &board, bool white);
inline BitB straightAttackersBB(const Board &board, bool white);
inline int getKingSquareIndex(const Board &board, bool white);

//--------------------- Zobrist hashing helpers

// returns an index from 0-15 for the 16 possible castling rights combinations
inline int castlingIndex(const Board &board)
{
    int index = 0;

    if (board.whiteCanKsCastle)
        index |= 1;
    if (board.whiteCanQsCastle)
        index |= 2;
    if (board.blackCanKsCastle)
        index |= 4;
    if (board.blackCanQsCastle)
        index |= 8;

    return index;
}

//--------------------- Position loading

// Call a FEN loader with the start FEN
void loadStartPosition(Board &board);
