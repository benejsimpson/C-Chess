#include "engine/zobrist.hpp"

#include <random>

Zobrist ZOBRIST;

static Hash random_hash()
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
            ZOBRIST.piece_square_hashes[piece][square] = random_hash();
        }
    }

    ZOBRIST.white_to_move_hash = random_hash();

    for (int i = 0; i < 16; ++i)
    {
        ZOBRIST.castling_hashes[i] = random_hash();
    }

    for (int file = 0; file < 8; ++file)
    {
        ZOBRIST.en_passant_file_hashes[file] = random_hash();
    }
}

// returns hash generated from current board position
Hash generate_hash(const Board &board)
{
    Hash hash = 0;
    BitB all_occup = all_occupancy(board);

    // pieces
    while (all_occup != 0)
    {
        const int sq = pop_lsb(all_occup);
        Piece piece = board.squares[sq];

        hash ^= ZOBRIST.piece_square_hashes[piece_to_bb_ind(piece)][sq];
    }

    // side to move
    if (board.white_to_move)
        hash ^= ZOBRIST.white_to_move_hash;

    // castling rights
    hash ^= ZOBRIST.castling_hashes[castling_index(board)];

    // en passant
    if (board.en_passant_square != -1)
    {
        int file = index_to_file(board.en_passant_square);
        hash ^= ZOBRIST.en_passant_file_hashes[file];
    }
    return hash;
}

void update_hash_for_moved_piece(Board &board,const Move &move)
{
    const int from = move_from(move);
    const int to = move_to(move);
    const bool capture = is_capture(board, move);

    // remove moved piece from old square
    board.hash ^= ZOBRIST.piece_square_hashes[piece_to_bb_ind(board.squares[from])][from];

    // add piece moved to new square
    board.hash ^= ZOBRIST.piece_square_hashes[piece_to_bb_ind(board.squares[to])][to];

    // remove captured piece
    if (capture)
    {
        board.hash ^= ZOBRIST.piece_square_hashes[piece_to_bb_ind(board.squares[to])][to];
    }

    // flip side to move
    board.hash ^= ZOBRIST.white_to_move_hash;
}

void update_hash_for_castling(Board &board, const Move &move)
{
    if (!is_castle(move))
        return;

    // flip castling rights
    board.hash ^= ZOBRIST.castling_hashes[castling_index(board)];
}

void update_hash_for_en_passant(Board &board, int old_ep_square, int new_ep_square)
{
    if (old_ep_square != -1)
    {
        const int old_file = index_to_file(old_ep_square);
        board.hash ^= ZOBRIST.en_passant_file_hashes[old_file];
    }

    if (new_ep_square != -1)
    {
        const int new_file = index_to_file(new_ep_square);
        board.hash ^= ZOBRIST.en_passant_file_hashes[new_file];
    }
}

int count_repetitions(const std::vector<Hash> &history, Hash current_hash)
{
    int count = 0;
    for (const Hash &hash : history)
    {
        if (hash == current_hash)
            ++count;
    }
    return count;
}