#pragma once

#include "core/move.hpp"

#include <cstdint>
#include <vector>

using BitEntry = uint64_t;

enum TTFlag
{
    EXACT,          // exact score
    LOWER_BOUND,    // score >= this (alpha cut-off)
    UPPER_BOUND     // score <= this (beta cut-off)
};

struct TTEntry
{
    BitEntry key;   // 64-bit hash key
    int depth;
    int score;
    uint8_t flag;
    Move best_move;
};

static std::vector<TTEntry> TT;
static size_t TT_SIZE = 0;

void tt_clear()
{
    for (size_t i = 0; i < TT_SIZE; ++i)
    {
        TT[i].key = 0;      // 0 = empty slot
        TT[i].depth = -1;   // invalid depth
        TT[i].score = 0;
        TT[i].flag = 0;
        TT[i].best_move = 0;
    }
}

void tt_init(size_t size)
{
    TT_SIZE = size;
    TT.resize(TT_SIZE);

    tt_clear();
}


bool tt_probe(BitEntry key, TTEntry& out)
{
    if (TT_SIZE == 0)
        return false;

    size_t index = key % TT_SIZE;
    const TTEntry& entry = TT[index];

    if (entry.depth >= 0 && entry.key == key)
    {
        out = entry;
        return true;
    }

    return false;
}

void tt_store(BitEntry key, const TTEntry& entry)
{
    if (TT_SIZE == 0)
        return;

    size_t index = key % TT_SIZE;

    if (TT[index].key != key || entry.depth >= TT[index].depth)
    {
        TT[index] = entry;
    }
}
