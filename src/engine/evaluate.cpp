#include "engine/evaluate.hpp"
#include "engine/tt.hpp"
#include "engine/psqt.hpp"

#include <cmath>
#include <array>

//--------------------- Material & PSQT Evaluation

inline int evaluate(const Board &board)
{
    if (isCheckmate(const_cast<Board &>(board)))
        return board.whiteToMove ? -INF : INF;

    return psqtScore(board);
}

inline int evaluateMaterial(const Board &board)
{
    int eval = 0;

    for (int i = 0; i < 5; i++)
    {
        eval +=
            (countBits(board.pieceBBs[W_BB[i]]) - countBits(board.pieceBBs[B_BB[i]])) * PIECE_MATERIAL_SCORE[i];
    }
    return eval;
}

// combines piece scoring and ptsq scoring
// updates arrays for sections of game (opening, middle, end)
// updates ptsq for castling
inline int psqtScore(const Board &board)
{
    int eval = 0;

    // get PSQT for piece types relative to phase of game
    const PSQT w_pawns_psqt = pawnPSQTForPhase(board, true);
    const PSQT b_pawns_psqt = pawnPSQTForPhase(board, false);
    const PSQT w_king_psqt = kingPSQTForPhase(board, true);
    const PSQT b_king_psqt = kingPSQTForPhase(board, false);
    const PSQT_Set pieces_psqt = piecePSQTForPhase(board);

    // copies of pawn bitboards
    BitB w_pawns = board.pieceBBs[pieceToBitboardIndex(WP)];
    BitB b_pawns = board.pieceBBs[pieceToBitboardIndex(BP)];

    // loop through white piece bitboards, add material score and PSQT value
    std::array<BitB, 4> w_pieces = {
        board.pieceBBs[pieceToBitboardIndex(WN)],
        board.pieceBBs[pieceToBitboardIndex(WB)],
        board.pieceBBs[pieceToBitboardIndex(WR)],
        board.pieceBBs[pieceToBitboardIndex(WQ)]};

    std::array<BitB, 4> b_pieces = {
        board.pieceBBs[pieceToBitboardIndex(BN)],
        board.pieceBBs[pieceToBitboardIndex(BB)],
        board.pieceBBs[pieceToBitboardIndex(BR)],
        board.pieceBBs[pieceToBitboardIndex(BQ)]};

    BitB w_king_pos = LsbIndex(board.pieceBBs[pieceToBitboardIndex(WK)]);
    BitB b_king_pos = LsbIndex(board.pieceBBs[pieceToBitboardIndex(BK)]);

    eval += (w_king_psqt[w_king_pos] - b_king_psqt[b_king_pos]);

    while (w_pawns != 0)
    {
        eval += PIECE_MATERIAL_SCORE[0] + w_pawns_psqt[popLSB(w_pawns)];
    }
    while (b_pawns != 0)
    {
        eval -= PIECE_MATERIAL_SCORE[0] + b_pawns_psqt[popLSB(b_pawns)];
    }

    for (int i = 0; i < 4; ++i)
    {
        BitB wPiece = w_pieces[i];
        BitB b_piece = b_pieces[i];

        while (wPiece != 0)
        {
            eval += PIECE_MATERIAL_SCORE[i + 1] + pieces_psqt[i][popLSB(wPiece)];
        }
        while (b_piece != 0)
        {
            eval -= PIECE_MATERIAL_SCORE[i + 1] + pieces_psqt[i][popLSB(b_piece)];
        }
    }
    return eval;
}

Move findBestMove(Board board, int baseDepth)
{
    int depth = baseDepth + depthBonus(board);
    MoveList moves = generateLegalMoves(board);

    // No legal moves: checkmate or stalemate
    if (moves.empty())
        return 0; // no move

    // Safety: never search below depth 1
    if (depth < 1)
        depth = 1;

    Move bestMove = moves[0];

    int bestScore = board.whiteToMove ? -INF : INF;

    for (Move move : moves)
    {
        Undo undo;
        makeMove(board, move, undo);

        int score = minimax(board, depth - 1, -INF, INF);

        undoMove(board, move, undo);

        if (board.whiteToMove && score > bestScore)
        {
            bestScore = score;
            bestMove = move;
        }
        else if (!board.whiteToMove && score < bestScore)
        {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

//--------------------- MiniMax & Search

// init with a = -inf, b = inf
int minimax(Board board, int depth, int alpha, int beta)
{
    const int og_alpha = alpha;
    const int og_beta = beta;

    TTEntry entry;
    if (ttProbe(board.hash, entry))
    {
        if (entry.depth >= depth)
        {
            if (entry.flag == EXACT)
                return entry.score;

            if (entry.flag == LOWER_BOUND)
                alpha = std::max(alpha, entry.score);

            else if (entry.flag == UPPER_BOUND)
                beta = std::min(beta, entry.score);

            if (alpha >= beta)
                return entry.score;
        }
    }

    if (depth <= 0)
        return evaluate(board);

    MoveList moves = generateLegalMoves(board);

    if (moves.empty())
    {
        if (isCheckmate(board)) // checkmate
        {
            return board.whiteToMove
                       ? depth - INF
                       : INF - depth;
        }
        else // stalemate
            return 0;
    }

    if (isDrawByRepetition(board))
        return 0;

    // order the moves based on their score given in movelist.hpp
    orderMoves(moves, board);

    int bestScore;
    Move bestMove = 0;

    if (board.whiteToMove) // maximise
    {
        bestScore = -INF;

        for (Move move : moves)
        {
            Undo undo;
            makeMove(board, move,undo);

            int eval = minimax(board, depth - 1, alpha, beta);

            undoMove(board, move, undo);

            if (eval > bestScore)
            {
                bestScore = eval;
                bestMove = move;
            }

            alpha = std::max(alpha, eval);

            if (beta <= alpha)
                break;
        }
    }

    else // minimise
    {
        bestScore = INF;

        for (Move move : moves)
        {
            Undo undo;
            makeMove(board, move,undo);

            int eval = minimax(board, depth - 1, alpha, beta);

            undoMove(board, move, undo);

            if (eval < bestScore)
            {
                bestScore = eval;
                bestMove = move;
            }

            beta = std::min(beta, eval);

            if (beta <= alpha)
                break;
        }
    }

    // STORE RESULT IN TRANSPOSITION TABLE
    TTEntry newEntry;
    newEntry.key = board.hash;
    newEntry.depth = depth;
    newEntry.score = bestScore;
    newEntry.bestMove = bestMove;

    if (bestScore <= og_alpha)
        newEntry.flag = UPPER_BOUND;
    else if (bestScore >= og_beta)
        newEntry.flag = LOWER_BOUND;
    else
        newEntry.flag = EXACT;

    ttStore(board.hash, newEntry);

    return bestScore;
}

int search(Board board, int depth)
{

    if (depth == 0) // depth limit hit -> return eval
        return evaluate(board);

    // get array of all legal moves from MoveList
    MoveList moves = generateLegalMoves(board);

    // CHECKMATE / STALEMATE
    if (moves.empty())
    {
        if (isCheckmate(board)) // checkmate
        {
            return board.whiteToMove
                       ? depth - INF
                       : INF - depth;
        }
        else // stalemate
            return 0;
    }

    if (isDrawByRepetition(board))
        return 0;

    int best = board.whiteToMove ? -INF : INF;

    for (Move move : moves)
    {
        Undo undo;
        makeMove(board, move,undo);

        int score = search(board, depth - 1);

        undoMove(board, move, undo);

        if (board.whiteToMove)
            best = std::max(best, score);
        else
            best = std::min(best, score);
    }
    return best;
}

//--------------------- Dynamic Depth Searching

int depthBonus(const Board &board)
{
    const int numPieces = countPiecesOnBoard(board);

    if (numPieces == 0)
        return 5;
    if (numPieces <= 4)
        return 2;
    else if (numPieces <= 8)
        return 1;
    return 0;
}

//--------------------- Quiescence
/*
    When depth limit reached, engine returns eval but there may be obvious tactical moves
    e.g. queen captures piece at depth = 0 -> eval = +5, but opponent can recapture next move -> eval = -4
    Instead, this will continue to evaluate positions with obvious tactical continuations, until no forcing moves remain
    Quiet position -> evaluate, Tactical position -> continue searching
*/

int quiescence(Board &board, int alpha, int beta)
{
    return 0;
}