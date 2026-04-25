#pragma once

#include "core/board.hpp"
#include "core/fen.hpp"
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

const int W_BB[5] = {0, 1, 2, 3, 4};
const int B_BB[5] = {6, 7, 8, 9, 10};

const int PIECES[5] = {P, N, B, R, Q};

inline int count_set_bits(uint64_t bb)
{
    return __builtin_popcountll(bb);
}

inline int evaluate(const Board &board)
{
    int eval = 0;

    for (int i = 0; i < 5; i++)
    {
        eval +=
            (count_set_bits(board.bitboards[W_BB[i]].bits) - count_set_bits(board.bitboards[B_BB[i]].bits)) * PIECES[i];
    }
    return eval;
}






struct EvalResult
{
    std::string fen;
    std::string stockfish_eval;
    int eval;
    int absolute_eval;
};

std::vector<EvalResult> evaluate_positions_from_csv(
    const std::string& file_path,
    const std::string& version);

bool write_eval_results_to_csv(
    const std::vector<EvalResult>& results,
    const std::string& file_path,
    const std::string& version);

bool evaluate_csv_positions(const std::string& version = "material_v1");
