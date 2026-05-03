#include "engine/zobrist.hpp"

#include <random>

Zobrist ZOBRIST;

static Hash randomHash()
{
    static std::mt19937_64 rng(123456789);
    return rng();
}

void init_zobrist()
{
    for (int piece = 0; piece < 12; ++piece)
    {
        for (int square = 0; square < 64; ++square)
        {
            ZOBRIST.pieceSquareHashes[piece][square] = randomHash();
        }
    }

    ZOBRIST.whiteToMoveHash = randomHash();

    for (int i = 0; i < 16; ++i)
    {
        ZOBRIST.castlingHashes[i] = randomHash();
    }

    for (int file = 0; file < 8; ++file)
    {
        ZOBRIST.enPassantFileHashes[file] = randomHash();
    }
}

// returns hash generated from current board position
Hash generateHash(const Board &board)
{
    Hash hash = 0;
    BitB allOccup = allOccupancyBB(board);

    // pieces
    while (allOccup != 0)
    {
        const int sq = popLSB(allOccup);
        Piece piece = board.squares[sq];

        hash ^= ZOBRIST.pieceSquareHashes[pieceToBitboardIndex(piece)][sq];
    }

    // side to move
    if (board.whiteToMove)
        hash ^= ZOBRIST.whiteToMoveHash;

    // castling rights
    hash ^= ZOBRIST.castlingHashes[castlingIndex(board)];

    // en passant
    if (board.enPassantSquare != -1)
    {
        int file = indexToFile(board.enPassantSquare);
        hash ^= ZOBRIST.enPassantFileHashes[file];
    }
    return hash;
}

// if castling rights change between moves, replace the old castling index with the new one
void updateCastlingHash(Board &board, int oldCastleIndex, int newCastleIndex)
{
    if (oldCastleIndex == newCastleIndex)
        return;

    // flip castling rights
    board.hash ^= ZOBRIST.castlingHashes[oldCastleIndex];
    board.hash ^= ZOBRIST.castlingHashes[newCastleIndex];
}

void updateEnPassantHash(Board &board, int oldEpSquare, int newEpSquare)
{
    if (oldEpSquare != -1)
    {
        const int oldFile = indexToFile(oldEpSquare);
        board.hash ^= ZOBRIST.enPassantFileHashes[oldFile];
    }

    if (newEpSquare != -1)
    {
        const int newFile = indexToFile(newEpSquare);
        board.hash ^= ZOBRIST.enPassantFileHashes[newFile];
    }
}

int countRepetitions(const std::vector<Hash> &history, Hash currentHash)
{
    int count = 0;
    for (const Hash &hash : history)
    {
        if (hash == currentHash)
            ++count;
    }
    return count;
}