#include "gui/game.hpp"

#include "core/fen.hpp"
#include "core/movegen.hpp"

namespace
{
constexpr const char* kStartFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq";
}

Game::Game()
{
    reset_to_start();
}

const Board& Game::get_board() const
{
    return board;
}

std::vector<Move> Game::get_legal_moves() const
{
    return generate_legal_moves(board);
}

std::vector<Move> Game::get_legal_moves_for_square(int square) const
{
    return generate_legal_moves_for_square(board, square);
}

bool Game::try_make_move(const Move& move)
{
    for (const Move& legal_move : get_legal_moves())
    {
        if (!same_move(legal_move, move))
            continue;

        apply_move(board, legal_move);
        last_move = legal_move;
        last_move_exists = true;
        return true;
    }

    return false;
}

void Game::reset_to_start()
{
    load_position_from_fen(kStartFen);
}

bool Game::load_position_from_fen(const std::string& fen)
{
    load_fen(board, fen);
    last_move_exists = false;
    return true;
}

std::string Game::get_fen() const
{
    return export_fen(board);
}

bool Game::white_to_move() const
{
    return board.white_to_move;
}

bool Game::is_check() const
{
    return is_in_check(board, board.white_to_move);
}

std::string Game::get_status_text() const
{
    const bool white_turn = board.white_to_move;
    const std::vector<Move> legal_moves = get_legal_moves();

    if (legal_moves.empty())
    {
        if (is_check())
            return white_turn ? "Checkmate: Black wins" : "Checkmate: White wins";

        return "Stalemate";
    }

    if (is_check())
        return white_turn ? "White to move - check" : "Black to move - check";

    return white_turn ? "White to move" : "Black to move";
}

bool Game::has_last_move() const
{
    return last_move_exists;
}

Move Game::get_last_move() const
{
    return last_move;
}
