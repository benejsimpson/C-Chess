#pragma once

#include "core/board.hpp"
#include "core/fen.hpp"
#include "core/move.hpp"
#include "core/movegen.hpp"
#include "core/movelist.hpp"
#include "core/makemove.hpp"
#include "core/piece.hpp"
#include "engine/PSQT.hpp"

#include <cstdint>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <array>

//--------------------- Material & PSQT Evaluation

inline int evaluate(const Board &board);
inline int psqtScore(const Board &board);
inline int evaluateMaterial(const Board &board);

//--------------------- MiniMax & Search

int search(Board board, int depth);
int minimax(Board board, int depth, int alpha, int beta);
Move findBestMove(Board board, int depth);

//--------------------- Dynamic Depth

int depthBonus(const Board &board);

//--------------------- Helpers

// returns square index when horizontally inverted
// changes perspective between white & black
inline int mirrorSquare(int square)
{
    return square ^ 56;
}
