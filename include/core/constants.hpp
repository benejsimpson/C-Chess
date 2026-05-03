#pragma once

#include <array>

using dMove = std::array<int,2>;

//--------------------- FEN

inline constexpr char START_FEN[58] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";

//--------------------- Piece Movement Displacement
using BitB = uint64_t;

inline constexpr std::array<dMove, 8> KNIGHT_MOVES =
    {{{1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1}}};
inline constexpr std::array<dMove, 8> KING_MOVES =
    {{{0, 1}, {1, 0}, {0, -1}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};
inline constexpr std::array<dMove, 4> DIAGONAL_MOVES =
    {{{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};
inline constexpr std::array<dMove, 4> STRAIGHT_MOVES =
    {{{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};

//--------------------- BitBoard Indices

inline constexpr int WHITE_BB_INDS[6] = {0, 1, 2, 3, 4, 5};
inline constexpr int BLACK_BB_INDS[6] = {6, 7, 8, 9, 10, 11};

//--------------------- Piece Material Values

inline constexpr int P_SCORE = 100;
inline constexpr int N_SCORE = 300;
inline constexpr int B_SCORE = 350;
inline constexpr int R_SCORE = 500;
inline constexpr int Q_SCORE = 900;
inline constexpr int INF = 1000000;

inline constexpr int W_BB[6] = {0, 1, 2, 3, 4, 5};
inline constexpr int B_BB[6] = {6, 7, 8, 9, 10, 11};

inline constexpr int PIECE_MATERIAL_SCORE[5] = {P_SCORE, N_SCORE, B_SCORE, R_SCORE, Q_SCORE};