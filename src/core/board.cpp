// src/core/board.cpp

#include "core/utils.hpp"
#include "core/board.hpp"
#include "core/fen.hpp"
#include "engine/zobrist.hpp"
#include <cctype>

using namespace std;

// Piece helpers

// put a piece on a square
// updates board.squares[] and piece's bitboard
void placePiece(Board &board, int square, Piece piece)
{
    if (!isValidIndex(square) || piece == Empty)
        return;

    board.squares[square] = piece;

    // update piece's 64-bitboard
    const int bbIndex = pieceToBitboardIndex(piece);
    if (bbIndex != -1)
    {
        set_bit(board.pieceBBs[bbIndex], square);
    }

    // updates hash for placed piece
    board.hash ^= ZOBRIST.pieceSquareHashes[pieceToBitboardIndex(piece)][square];
}

// removes a piece from a square
// updates board.squares[] and piece's bitboard
void removePiece(Board &board, int square)
{
    if (!isValidIndex(square))
        return;

    const Piece piece = board.squares[square];
    if (piece == Empty)
        return;

    board.squares[square] = Empty;

    // unset bit in piece's bitboard
    clear_bit(
        board.pieceBBs[pieceToBitboardIndex(piece)],
        square);

    // updates hash for removed piece
    board.hash ^= ZOBRIST.pieceSquareHashes[pieceToBitboardIndex(piece)][square];
}

// removes piece and places it in new position
// updates board.squares[] and piece's bitboard
void movePiece(Board &board, int from, int to)
{
    // find the piece on the square moving from (the piece being moved)
    Piece piece = board.squares[from];

    // remove any captured piece on square moving to
    Piece captured = board.squares[to];
    if (!isPieceEmpty(captured))
    {
        removePiece(board, to);
    }

    // remove moving piece from original position and place on new square
    removePiece(board, from);
    placePiece(board, to, piece);
}

// Bitboard helpers

// returns bb of all squares occupied by white
inline BitB whiteOccupancyBB(const Board &board)
{
    return board.pieceBBs[pieceToBitboardIndex(WP)] |
           board.pieceBBs[pieceToBitboardIndex(WN)] |
           board.pieceBBs[pieceToBitboardIndex(WB)] |
           board.pieceBBs[pieceToBitboardIndex(WR)] |
           board.pieceBBs[pieceToBitboardIndex(WQ)] |
           board.pieceBBs[pieceToBitboardIndex(WK)];
}

// returns bb of all squares occupied by black
inline BitB blackOccupancyBB(const Board &board)
{
    return board.pieceBBs[pieceToBitboardIndex(BP)] |
           board.pieceBBs[pieceToBitboardIndex(BN)] |
           board.pieceBBs[pieceToBitboardIndex(BB)] |
           board.pieceBBs[pieceToBitboardIndex(BR)] |
           board.pieceBBs[pieceToBitboardIndex(BQ)] |
           board.pieceBBs[pieceToBitboardIndex(BK)];
}

// returns bb of all squares occupied by either side
inline BitB allOccupancyBB(const Board &board)
{
    return whiteOccupancyBB(board) | blackOccupancyBB(board);
}

// returns index of [white] king
inline int getKingSquareIndex(const Board &board, bool white)
{
    const BitB king_bb = board.pieceBBs[pieceToBitboardIndex(white ? WK : BK)];
    return king_bb ? LsbIndex(king_bb) : -1;
}

// returns bb of all squares with [white] queen & bishop
inline BitB diagonalAttackersBB(const Board &board, bool white)
{
    return board.pieceBBs[pieceToBitboardIndex(white ? WB : BB)] |
           board.pieceBBs[pieceToBitboardIndex(white ? WQ : BQ)];
}

// returns bb of all squares with [white] queen & rook
inline BitB straightAttackersBB(const Board &board, bool white)
{
    return board.pieceBBs[pieceToBitboardIndex(white ? WR : BR)] |
           board.pieceBBs[pieceToBitboardIndex(white ? WQ : BQ)];
}

// Board setup / utility

// clears the board and loads the starting position
void resetBoard(Board &board)
{
    clearBoard(board);
    loadStartPosition(board);
}

// clears the board of all pieces and resets all board state
void clearBoard(Board &board)
{
    // set all squares to empty
    for (int i = 0; i < 64; i++)
    {
        board.squares[i] = Empty;
    }
    // clear bitboards for each piece
    for (int i = 0; i < 12; i++)
    {
        board.pieceBBs[i] = EMPTY_BB;
    }

    // reset side to move
    board.whiteToMove = true;

    // clear castling rights
    board.whiteCanKsCastle = false;
    board.whiteCanQsCastle = false;
    board.blackCanKsCastle = false;
    board.blackCanQsCastle = false;

    board.whiteHasKsCastled = false;
    board.whiteHasQsCastled = false;
    board.blackHasKsCastled = false;
    board.blackHasQsCastled = false;

    // set en-passant square = -1
    board.enPassantSquare = -1;

    // reset attack masks
    board.whiteAttacksMask = 0;
    board.blackAttacksMask = 0;

    // reset move counter
    board.fullmoveNumber = 1;

    // reset hashing state
    board.hash = 0;
    board.positionHistory.clear();
}

// loads a position from START_FEN string
void loadStartPosition(Board &board)
{
    loadFEN(board, START_FEN);
    board.whiteCanKsCastle = true;
    board.whiteCanQsCastle = true;
    board.blackCanKsCastle = true;
    board.blackCanQsCastle = true;

    board.whiteHasKsCastled = false;
    board.whiteHasQsCastled = false;
    board.blackHasKsCastled = false;
    board.blackHasQsCastled = false;
}
