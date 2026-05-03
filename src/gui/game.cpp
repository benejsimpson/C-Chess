#include "gui/game.hpp"

#include "core/fen.hpp"
#include "core/movegen.hpp"
#include "core/makemove.hpp"
#include "engine/evaluate.hpp"

bool Game::make_ai_move(int depth)
{
    MoveList moves = getLegalMoves();

    if (moves.empty())
        return false;

    Move bestMove = findBestMove(board, depth);

    applyMove(board, bestMove);
    last_move = bestMove;
    last_move_exists = true;

    return true;
}

Game::Game()
{
    resetToStart();
}

const Board &Game::getBoard() const
{
    return board;
}

MoveList Game::getLegalMoves() const
{
    return generateLegalMoves(board);
}

MoveList Game::getLegalMovesForSquare(int square) const
{
    return generateLegalMovesForSquare(board, square);
}

bool Game::tryMakeMove(const Move &move)
{
    for (const Move &legal_move : getLegalMoves())
    {
        if (!sameMove(legal_move, move))
            continue;

        applyMove(board, legal_move);
        last_move = legal_move;
        last_move_exists = true;
        return true;
    }

    return false;
}

void Game::resetToStart()
{
    loadPositionFromFen(START_FEN);
}

bool Game::loadPositionFromFen(const std::string &fen)
{
    loadFEN(board, fen);
    last_move_exists = false;
    return true;
}

std::string Game::get_fen() const
{
    return exportFEN(board);
}

bool Game::whiteToMove() const
{
    return board.whiteToMove;
}

bool Game::isCheck() const
{
    return isInCheck(board, board.whiteToMove);
}

std::string Game::getStatusText() const
{
    const bool whiteTurn = board.whiteToMove;
    MoveList legalMoves = getLegalMoves();

    if (legalMoves.empty())
    {
        if (isCheck())
            return whiteTurn ? "Checkmate: Black wins" : "Checkmate: White wins";

        return "Stalemate";
    }

    if (isCheck())
        return whiteTurn ? "White to move - check" : "Black to move - check";

    return whiteTurn ? "White to move" : "Black to move";
}

bool Game::hasLastMove() const
{
    return last_move_exists;
}

Move Game::getLastMove() const
{
    return last_move;
}
