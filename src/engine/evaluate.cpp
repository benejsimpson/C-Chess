#include "engine/evaluate.hpp"
#include "engine/tt.hpp"

#include <cmath>

using namespace std;

inline int evaluate(const Board &board)
{
    if (is_checkmate(const_cast<Board&>(board)))
        return board.white_to_move ? -MATE_SCORE : MATE_SCORE;

    return psqt_score(board);
}

inline int evaluate_material(const Board &board)
{
    int eval = 0;

    for (int i = 0; i < 5; i++)
    {
        eval +=
            (count_bits(board.bitboards[W_BB[i]]) - count_bits(board.bitboards[B_BB[i]])) * PIECE_MATERIAL_SCORE[i];
    }
    return eval;
}

inline int psqt_score(const Board &board)
{
    int eval = 0;

    // loop through each piece type
    for (int i = 0; i < 6; i++)
    {
        // copy of white piece bitboard
        BitB w_copy = board.bitboards[WHITE_BB_INDS[i]];

        while (w_copy != 0)
        {
            // add piece square value for piece to eval
            eval += PIECE_MATERIAL_SCORE[i] + OPENING_PSQT[i][pop_lsb(w_copy)];
        }

        // copy of black piece bitboard
        BitB b_copy = board.bitboards[BLACK_BB_INDS[i]];
        while (b_copy != 0)
        {
            // subtract piece square value for piece to eval
            eval -= PIECE_MATERIAL_SCORE[i] + OPENING_PSQT[i][mirror_square(pop_lsb(b_copy))];
        }
    }
    return eval;
}



Move find_best_move(Board board, int depth)
{
    MoveList moves = generate_legal_moves(board);

    // No legal moves: checkmate or stalemate
    if (moves.empty())
        return 0; // no move

    // Safety: never search below depth 1
    if (depth < 1)
        depth = 1;

    Move best_move = moves[0];

    int best_score = board.white_to_move ? -INF : INF;

    for (Move move : moves)
    {
        Board copy = board;
        apply_move(copy, move);

        int score = minimax(copy, depth - 1, -INF, INF);

        if (board.white_to_move && score > best_score)
        {
            best_score = score;
            best_move = move;
        }
        else if (!board.white_to_move && score < best_score)
        {
            best_score = score;
            best_move = move;
        }
    }

    return best_move;
}



// MINIMAX ALGORITHM
// init with a = -inf, b = inf
int minimax(Board board, int depth, int alpha, int beta)
{
    const int og_alpha = alpha;
    const int og_beta = beta;

    TTEntry entry;
    if (tt_probe(board.hash,entry))
    {
        if (entry.depth >= depth)
        {
            if (entry.flag == EXACT)
                return entry.score;

            if (entry.flag == LOWER_BOUND)
                alpha = max(alpha, entry.score);

            else if (entry.flag == UPPER_BOUND)
                beta = min(beta, entry.score);

            if (alpha >= beta)
                return entry.score;
        }
    }

    if (depth <= 0)
        return evaluate(board);

    MoveList moves = generate_legal_moves(board);

    if (moves.empty())
    {
        if (is_checkmate(board)) // checkmate
        {
            return board.white_to_move
            ? depth - MATE_SCORE
            : MATE_SCORE - depth;
        }
        else // stalemate
            return 0;
    }

    if (is_draw_by_repetition(board))
        return 0;

    int best_score;
    Move best_move = 0;

    if (board.white_to_move) // maximise
    {
        best_score = -INF;

        for (Move move : moves)
        {
            Board copy = board;
            apply_move(copy, move);

            int eval = minimax(copy, depth - 1, alpha, beta);

            if (eval > best_score)
            {
                best_score = eval;
                best_move = move;
            }

            alpha = max(alpha, eval);

            if (beta <= alpha)
                break;
        }
    }

    else // minimise
    {
        best_score = INF;

        for (Move move : moves)
        {
            Board copy = board;
            apply_move(copy, move);

            int eval = minimax(copy, depth - 1, alpha, beta);

            if (eval < best_score)
            {
                best_score = eval;
                best_move = move;
            }

            beta = min(beta, eval);

            if (beta <= alpha)
                break;
        }
    }

    // STORE RESULT IN TRANSPOSITION TABLE
    TTEntry new_entry;
    new_entry.key = board.hash;
    new_entry.depth = depth;
    new_entry.score = best_score;
    new_entry.best_move = best_move;

    if (best_score <= og_alpha)
        new_entry.flag = UPPER_BOUND;
    else if (best_score >= og_beta)
        new_entry.flag = LOWER_BOUND;
    else
        new_entry.flag = EXACT;

    tt_store(board.hash, new_entry);

    return best_score;
}



int search(Board board, int depth)
{

    if (depth == 0) // depth limit hit -> return eval
        return evaluate(board);

    // get array of all legal moves from MoveList
    MoveList moves = generate_legal_moves(board);

                                                            // CHECKMATE / STALEMATE
    if (moves.empty())
    {
        if (is_checkmate(board)) // checkmate
        {
            return board.white_to_move
            ? depth - MATE_SCORE
            : MATE_SCORE - depth;
        }
        else // stalemate
            return 0;
    }

    if (is_draw_by_repetition(board))
        return 0;
    
    int best = board.white_to_move ? -INF : INF;

    for (Move move : moves)
    {
        Board copy = board;
        apply_move(copy, move);

        int score = search(copy, depth - 1);

        if (board.white_to_move)
            best = max(best, score);
        else
            best = min(best, score);
    }
    return best;
}

