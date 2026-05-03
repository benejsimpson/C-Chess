#pragma once

#include "utils.hpp"
#include "constants.hpp"

#include <cstdint>
#include <array>

using MaskArray = std::array<BitB, 64>;
using BitB = uint64_t;

//--------------------- Constants

constexpr uint8_t COLOUR_MASK = 0b11000;
constexpr uint8_t TYPE_MASK = 0b00111;

//--------------------- Passed Pawn Mask

constexpr BitB passedPawnMask(int square, bool white)
{
    const int start_rank = indexToRank(square);
    const int file = indexToFile(square);

    BitB relevant_files = fileMask(file);

    // Add file to the left, if it exists
    if (file > 0)
        relevant_files |= fileMask(file - 1);

    // Add file to the right, if it exists
    if (file < 7)
        relevant_files |= fileMask(file + 1);

    BitB forward_ranks = EMPTY_BB;

    if (white)
    {
        for (int rank = start_rank + 1; rank <= 7; ++rank)
            forward_ranks |= rankMask(rank);
    }
    else
    {
        for (int rank = start_rank - 1; rank >= 0; --rank)
            forward_ranks |= rankMask(rank);
    }

    return relevant_files & forward_ranks;
}

static constexpr MaskArray buildPassedPawnMasks(bool white)
{
    MaskArray table{};
    for (int sq = 0; sq < 64; sq++)
    {
        table[sq] = passedPawnMask(sq, white);
    }
    return table;
}

inline constexpr MaskArray WHITE_PASSED_PAWN_MASKS = buildPassedPawnMasks(true);
inline constexpr MaskArray BLACK_PASSED_PAWN_MASKS = buildPassedPawnMasks(false);

// Piece definitions
enum Piece : uint8_t
{
    // pieces represented as integers from binary with 2-bit colour and 3-bit type
    // white = 01XXX, black = 10XXX
    // piece types are XXX from PAWN = 1 -> KING = 6
    Empty = 0,

    WHITE = 0b01000,
    BLACK = 0b10000,

    PAWN = 0b00001,
    KNIGHT = 0b00010,
    BISHOP = 0b00011,
    ROOK = 0b00100,
    QUEEN = 0b00101,
    KING = 0b00110,

    WP = WHITE | PAWN,
    WN = WHITE | KNIGHT,
    WB = WHITE | BISHOP,
    WR = WHITE | ROOK,
    WQ = WHITE | QUEEN,
    WK = WHITE | KING,

    BP = BLACK | PAWN,
    BN = BLACK | KNIGHT,
    BB = BLACK | BISHOP,
    BR = BLACK | ROOK,
    BQ = BLACK | QUEEN,
    BK = BLACK | KING
};

// converts a Piece into bitboard index
// W: P=0, N=1, B=2, R=3, Q=4, K=5
// B: p=6, n=7, b=8, r=9, q=10, k=11
inline constexpr int pieceToBitboardIndex(Piece piece)
{
    switch (piece)
    {
    case WP:
        return 0;
    case WN:
        return 1;
    case WB:
        return 2;
    case WR:
        return 3;
    case WQ:
        return 4;
    case WK:
        return 5;
    case BP:
        return 6;
    case BN:
        return 7;
    case BB:
        return 8;
    case BR:
        return 9;
    case BQ:
        return 10;
    case BK:
        return 11;
    default:
        return -1;
    }
}

//--------------------- Piece helpers

// true if piece is a white piece
inline constexpr bool isWhitePiece(Piece piece)
{
    return (uint8_t(piece) & COLOUR_MASK) == WHITE;
}

// true if piece is a black piece
inline constexpr bool isBlackPiece(Piece piece)
{
    return (uint8_t(piece) & COLOUR_MASK) == BLACK;
}

inline constexpr bool isOpponentPieces(Piece piece1, Piece piece2)
{
    return piece1 != Empty &&
           piece2 != Empty &&
           isWhitePiece(piece1) != isWhitePiece(piece2);
}

// gets the int value of piece
inline constexpr Piece getPieceType(Piece piece)
{
    return Piece(uint8_t(piece) & TYPE_MASK);
}

inline constexpr bool isPieceEmpty(Piece piece)
{
    return piece == Empty;
}

//--------------------- FEN helpers

// converts Piece p -> char representation in FEN
// white -> upper, black -> lower
char pieceToChar(Piece piece)
{
    switch (piece)
    {
    case WP:
        return 'P';
    case WN:
        return 'N';
    case WB:
        return 'B';
    case WR:
        return 'R';
    case WQ:
        return 'Q';
    case WK:
        return 'K';

    case BP:
        return 'p';
    case BN:
        return 'n';
    case BB:
        return 'b';
    case BR:
        return 'r';
    case BQ:
        return 'q';
    case BK:
        return 'k';

    default:
        return '.';
    }
}

Piece charToPiece(char c)
{
    switch (c)
    {
    case 'P':
        return WP;
    case 'N':
        return WN;
    case 'B':
        return WB;
    case 'R':
        return WR;
    case 'Q':
        return WQ;
    case 'K':
        return WK;

    case 'p':
        return BP;
    case 'n':
        return BN;
    case 'b':
        return BB;
    case 'r':
        return BR;
    case 'q':
        return BQ;
    case 'k':
        return BK;

    default:
        return Empty;
    }
}
