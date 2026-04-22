#pragma once
#include <vector>
#include <cstdint>
#include "utils.h"
#include "board.hpp"

// ---------------------------------
// Move flags
// ---------------------------------

enum MoveFlag : uint8_t
{
    QUIET = 0,      // normal move
    CAPTURE,

    DOUBLE_PAWN,    // pawn moves 2 squares
    EN_PASSANT,

    KING_CASTLE,    // king side castle
    QUEEN_CASTLE,   // queen side castle

    PROMOTION,      // pawn promotion (no capture)
    PROMO_CAPTURE   // promotion with capture
};


// ---------------------------------
// Move structure
// ---------------------------------

struct Move
{
    int from; // index of square
    int to; // index of square

    Piece piece;       // piece moving
    Piece captured;    // captured piece

    MoveFlag flag;     // special move type

    Piece promotion;   // promotion piece (if any, else Empty)

};


// ---------------------------------
// Helper functions
// ---------------------------------

// create a basic move
inline Move make_move(int from, int to, Piece piece, Piece captured)
{
    return Move{
        from,
        to,
        piece,
        captured,
        (captured == Empty ? QUIET : CAPTURE),
        Empty
    };
}

// check if move is capture
inline bool is_capture(const Move& m)
{
    return m.captured != Empty;
}

// check if move is promotion
inline bool is_promotion(const Move& m)
{
    return m.flag == PROMOTION || m.flag == PROMO_CAPTURE;
}


