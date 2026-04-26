#include "engine/evaluate.hpp"
#include <cmath>

using namespace std;

inline int evaluate(const Board &board)
{
    return evaluate_material(board) + psqt_score(board);
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
            eval += ALL_PSQT[i][pop_lsb(w_copy)];
        }

        // copy of black piece bitboard
        BitB b_copy = board.bitboards[BLACK_BB_INDS[i]];
        while (b_copy != 0)
        {
            // subtract piece square value for piece to eval
            eval -= ALL_PSQT[i][mirror_square(pop_lsb(b_copy))];
        }
    }
    return eval;
}

Move find_best_move(Board board, int depth)
{
    vector<Move> moves = generate_legal_moves(board);

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
        if (!board.white_to_move && score < best_score)
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
    if (depth == 0)
        return evaluate(board);

    vector<Move> moves = generate_legal_moves(board);

    // CHECKMATE / STALEMATE
    if (moves.empty())
    {
        if (is_checkmate(board)) // checkmate
        {
            return board.white_to_move ? depth - MATE_SCORE : MATE_SCORE - depth;
        }
        else // stalemate
            return 0;
    }

    if (board.white_to_move) // maximise
    {
        int max_eval = -INF;
        for (Move move : moves)
        {
            Board copy = board;
            apply_move(copy, move);

            int eval = minimax(copy, depth - 1, alpha, beta);
            max_eval = max(max_eval, eval);
            alpha = max(alpha, eval);
            if (beta <= alpha)
                break;
        }
        return max_eval;
    }

    else // minimise
    {
        int min_eval = INF;
        for (Move move : moves)
        {
            Board copy = board;
            apply_move(copy, move);

            int eval = minimax(copy, depth - 1, alpha, beta);
            min_eval = min(min_eval, eval);
            beta = min(beta, eval);
            if (beta <= alpha)
                break;
        }
        return min_eval;
    }
}

int search(Board board, int depth)
{
    if (depth == 0) // depth limit hit -> return value
        return evaluate(board);

    vector<Move> moves = generate_legal_moves(board); // get vector of all legal moves

    // CHECKMATE / STALEMATE
    if (moves.empty())
    {
        if (is_checkmate(board)) // checkmate
        {
            return board.white_to_move ? depth - MATE_SCORE : MATE_SCORE - depth;
        }
        else // stalemate
            return 0;
    }

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

