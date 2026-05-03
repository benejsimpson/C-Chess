#pragma once

#include "core/move.hpp"

#include <cstdint>
#include <vector>

using BitEntry = uint64_t;

enum TTFlag
{
    EXACT,       // exact score
    LOWER_BOUND, // score >= this (alpha cut-off)
    UPPER_BOUND  // score <= this (beta cut-off)
};

struct TTEntry
{
    BitEntry key; // 64-bit hash key
    int depth;
    int score;
    uint8_t flag;
    Move bestMove;
};

static std::vector<TTEntry> TT;
static size_t TT_SIZE = 0;

void ttClear()
{
    for (size_t i = 0; i < TT_SIZE; ++i)
    {
        TT[i].key = 0;    // 0 = empty slot
        TT[i].depth = -1; // invalid depth
        TT[i].score = 0;
        TT[i].flag = 0;
        TT[i].bestMove = 0;
    }
}

void ttInit(size_t size)
{
    TT_SIZE = size;
    TT.resize(TT_SIZE);

    ttClear();
}

bool ttProbe(BitEntry key, TTEntry &out)
{
    if (TT_SIZE == 0)
        return false;

    size_t index = key % TT_SIZE;
    const TTEntry &entry = TT[index];

    if (entry.depth >= 0 && entry.key == key)
    {
        out = entry;
        return true;
    }

    return false;
}

void ttStore(BitEntry key, const TTEntry &entry)
{
    if (TT_SIZE == 0)
        return;

    size_t index = key % TT_SIZE;

    if (TT[index].key != key || entry.depth >= TT[index].depth)
    {
        TT[index] = entry;
    }
}
