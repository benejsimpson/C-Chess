#pragma once
#include <cstdint>
#include "utils.h"
#include "board.hpp"

using Move = uint16_t;

constexpr int FLAG_SHIFT = 12;
constexpr int TO_SHIFT = 6;
constexpr int FROM_SHIFT = 0;

constexpr uint16_t FLAG_MASK = 0b1111000000000000;
constexpr uint16_t TO_MASK = 0b0000111111000000;
constexpr uint16_t FROM_MASK = 0b0000000000111111;

// Move flags
enum MoveFlag : uint8_t
{
    QUIET = 0, // normal move
    CAPTURE,   // capture

    KING_CASTLE,  // king side castle
    QUEEN_CASTLE, // queen side castle

    DOUBLE_PAWN, // pawn double step
    EN_PASSANT,  // en passant

    N_PROMO, // pawn promote to knight
    B_PROMO, // pawn promote to bishop
    R_PROMO, // pawn promote to rook
    Q_PROMO, // pawn promote to queen
};

                                                                    // Move structure

inline Move create_move(int from, int to, int flag)
{
    // Stores a move as 16-bit representation:
    // FFFF TTTTTT SSSSSS
    // Flag Target Start
    // Unpack with : get_[from, to, flag](move)
    return (from << FROM_SHIFT) |
           (to << TO_SHIFT) |
           (flag << FLAG_SHIFT);
}

                                                                    // Move parsing

// takes 16 bit move representation
// returns index of square moved from
inline const int move_from(Move move)
{
    return (move >> FROM_SHIFT) & 0x3F; // 6 bits
}
// takes 16 bit move representation
// returns index of square moved to
inline const int move_to(Move move)
{
    return (move >> TO_SHIFT) & 0x3F;
}
// takes 16 bit move representation
// returns int of MoveFlag
inline const int move_flag(Move move)
{
    return (move >> FLAG_SHIFT) & 0xF; // 4 bits
}

                                                                    // Move flag helpers
inline bool is_promotion_flag(int flag)
{
    return flag >= N_PROMO && flag <= Q_PROMO;
}

inline bool is_castle(Move move)
{
    return move_flag(move) == KING_CASTLE ||
           move_flag(move) == QUEEN_CASTLE;
}

inline bool is_en_passant(Move move)
{
    return move_flag(move) == EN_PASSANT;
}



                                                                    // Piece helper functions
inline Piece promotion_piece_from_flag(MoveFlag flag, bool white)
{
    switch (flag)
    {
    case Q_PROMO:
        return white ? WQ : BQ;
    case R_PROMO:
        return white ? WR : BR;
    case B_PROMO:
        return white ? WB : BB;
    case N_PROMO:
        return white ? WN : BN;

    default:
        return Empty;
    }
}