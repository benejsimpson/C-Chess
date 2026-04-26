#pragma once

#include "core/board.hpp"
#include "core/fen.hpp"
#include "core/move.hpp"
#include "core/movegen.hpp"

#include <cstdint>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>

const int P = 100;
const int N = 300;
const int B = 350;
const int R = 500;
const int Q = 900;
const int MATE_SCORE = 100000;
const int INF = 1000000;

const int W_BB[6] = {0, 1, 2, 3, 4, 5};
const int B_BB[6] = {6, 7, 8, 9, 10,11};

const int PIECE_MATERIAL_SCORE[5] = {P, N, B, R, Q};

inline int evaluate(const Board &board);

Move find_best_move(Board board, int depth);

int minimax(Board board, int depth, int alpha, int beta);
inline int psqt_score(const Board &board);
inline int evaluate_material(const Board &board);


constexpr int ALL_PSQT[6][64] =
{
    { // PAWNS
        0,   0,   0,   0,   0,   0,   0,   0,   // 1
        5,   5,   5, -10, -10,   5,   5,   5,   // 2
        5,   0,   0,   5,   5,   0,   0,   5,   // 3
        5,   5,   5,  20,  20,   5,   5,   5,   // 4
        10, 10,  15,  60,  60,  15,  10,  10,   // 5
        20, 20,  20,  30,  30,  30,  20,  20,   // 6
        30, 30,  30,  40,  40,  30,  30,  30,   // 7
        0,   0,   0,   0,   0,   0,   0,   0    // 8
    },
    { // KNIGHTS
        -5, -10, -5, -5, -5, -5, -10, -5,
        -5,   0,  0,  5,  5,  0,   0, -5,
        -5,   5, 10, 10, 10, 10,   5, -5,
        -5,   5, 10, 15, 15, 10,   5, -5,
        -5,   5, 10, 15, 15, 10,   5, -5,
        -5,   5, 10, 10, 10, 10,   5, -5,
        -5,   0,  0, 10, 10,  0,   0, -5,
        -5,  -5, -5, -5, -5, -5,  -5, -5
    },
    { // BISHOPS
        0,   0, -10,   0,   0, -10,   0,   0,
        0,  10,   0,  10,  10,   0,  10,   0,
        0,  10,   0,  10,  10,   0,  10,   0,
        5,   0,  10,   0,   0,  10,   0,   5,
        0,  10,   0,   0,   0,   0,  10,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0
    },
    { // ROOKS
        0,   0,   0,  10,  10,   5,   0,   0,
        0,   0,   0,  10,  10,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        10, 10, 10,  10,  10,  10, 10, 10,
        10, 10, 10,  10,  10,  10, 10, 10
    },
    { // QUEENS
        -20, -10, -10,  0,  0, -10, -10, -20,
        -10,   0,   5,  0,  0,   0,   0, -10,
        -10,   5,   5,  5,  5,   5,   0, -10,
        -5,    0,   5,  5,  5,   5,   0,  -5,
        -5,    0,   5,  5,  5,   5,   0,  -5,
        -10,   0,   5,  5,  5,   5,   0, -10,
        -10,   0,   0,  0,  0,   0,   0, -10,
        -20, -10, -10, -5, -5, -10, -10, -20
    },
    { // KINGS
        0, 0, 10, -5, -5, -5, 10, 0,
        0, 0,  0, -5, -5, -5,  0, 0,
        0, 0,  0,  0,  0,  0,  0, 0,
        0, 0,  0,  0,  0,  0,  0, 0,
        0, 0,  0,  0,  0,  0,  0, 0,
        0, 0,  0,  0,  0,  0,  0, 0,
        0, 0,  0,  0,  0,  0,  0, 0,
        0, 0,  0,  0,  0,  0,  0, 0
    }
};


inline int mirror_square(int square)
{
    return square ^ 56;
}

int search(Board board, int depth);

