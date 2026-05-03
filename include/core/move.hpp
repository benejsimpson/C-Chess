#pragma once
#include <cstdint>
#include "core/utils.hpp"
#include "core/board.hpp"

using Move = uint16_t;

//--------------------- Constants

constexpr int FLAG_SHIFT = 12;
constexpr int TO_SHIFT = 6;
constexpr int FROM_SHIFT = 0;

constexpr uint16_t FLAG_MASK = 0b1111000000000000;
constexpr uint16_t TO_MASK = 0b0000111111000000;
constexpr uint16_t FROM_MASK = 0b0000000000111111;

//--------------------- Move flags
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

    CHECK,
};

//--------------------- Move structure

inline Move createMove(int from, int to, int flag)
{
    // Stores a move as 16-bit representation:
    // FFFF TTTTTT SSSSSS
    // Flag Target Start
    // Unpack with : get_[from, to, flag](move)
    return (from << FROM_SHIFT) |
           (to << TO_SHIFT) |
           (flag << FLAG_SHIFT);
}

//--------------------- Move parsing

// takes 16 bit move representation
// returns index of square moved from
inline const int moveFrom(Move move)
{
    return (move >> FROM_SHIFT) & 0x3F; // 6 bits
}
// takes 16 bit move representation
// returns index of square moved to
inline const int moveTo(Move move)
{
    return (move >> TO_SHIFT) & 0x3F;
}
// takes 16 bit move representation
// returns int of MoveFlag
inline const int moveFlag(Move move)
{
    return (move >> FLAG_SHIFT) & 0xF; // 4 bits
}

// Move flag helpers
inline bool isPromotionFlag(int flag)
{
    return flag >= N_PROMO && flag <= Q_PROMO;
}

inline bool is_castle(const Move &move)
{
    return moveFlag(move) == KING_CASTLE ||
           moveFlag(move) == QUEEN_CASTLE;
}

inline bool is_en_passant(const Move &move)
{
    return moveFlag(move) == EN_PASSANT;
}

inline bool isCapture(const Board &board, const Move &move)
{
    const int flag = moveFlag(move);

    return flag == CAPTURE ||
           flag == EN_PASSANT ||
           board.squares[moveTo(move)] != Empty;
}

//--------------------- Move helper functions

inline Piece promotionPieceFromFlag(MoveFlag flag, bool white)
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

inline std::string moveflag_to_string(MoveFlag flag)
{
    switch (flag)
    {
    case QUIET:
        return "-";
    case CAPTURE:
        return "Capture";
    case KING_CASTLE:
        return "King Castle";
    case QUEEN_CASTLE:
        return "Queen Castle";
    case DOUBLE_PAWN:
        return "Double Pawn";
    case EN_PASSANT:
        return "En Passant";
    case N_PROMO:
        return "Promote Knight";
    case B_PROMO:
        return "Promote Bishop";
    case R_PROMO:
        return "Promote Rook";
    case Q_PROMO:
        return "Promote Queen";
    default:
        return "UNKNOWN FLAG";
    }
}

inline void print_move(const Board &board, const Move &move)
{
    std::cout << pieceToChar(board.squares[moveFrom(move)]) << " " << squareToName(moveFrom(move))
              << " -> " << squareToName(moveTo(move)) << " " << pieceToChar(board.squares[moveTo(move)])
              << " | " << moveflag_to_string((MoveFlag)moveFlag(move)) << '\n';
}