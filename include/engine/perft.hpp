// include/engine/perft.hpp
#pragma once
#include <cstdint>
#include "core/board.hpp"

uint64_t perft(const Board& board, int depth);
