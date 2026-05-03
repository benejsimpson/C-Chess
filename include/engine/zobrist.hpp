#pragma once

#include "core/board.hpp"
#include "core/move.hpp"
#include <array>
#include <cstdint>

using Hash = uint64_t;

struct Zobrist
{
    std::array<std::array<Hash, 64>, 12> pieceSquareHashes; // [piece][square]

    // 16 possible castling rights combinations (KQkq) : 0000 to 1111
    std::array<Hash, 16> castlingHashes{}; // [castling_rights]

    // 8 possible en passant files (a-h) : 0000 to 1111
    std::array<Hash, 8> enPassantFileHashes{}; // [file]

    Hash whiteToMoveHash = 0;
};

extern Zobrist ZOBRIST;

void init_zobrist();

Hash generateHash(const Board &board);

void updateCastlingHash(Board &board, int oldCastleIndex, int newCastleIndex);
void updateEnPassantHash(Board &board, int oldEpSquare, int newEpSquare);

// returns the number repeated moves from hash history
int countRepetitions(const std::vector<Hash> &history, Hash currentHash);

// returns true if the current position is drawn by repetition
// uses hashes stored in board.positionHistory to count number of times current position has occurred in the game
inline bool isDrawByRepetition(const Board &board)
{
    return countRepetitions(board.positionHistory, board.hash) >= 3;
}
