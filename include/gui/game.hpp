#pragma once
#include <string>
#include <vector>
#include "core/board.hpp"
#include "core/move.hpp"

class Game
{
private:
    Board board;
    Move last_move;
    bool last_move_exists = false;

public:
    Game();

    const Board& get_board() const;

    std::vector<Move> get_legal_moves() const;
    std::vector<Move> get_legal_moves_for_square(int square) const;

    bool try_make_move(const Move& move);

    void reset_to_start();
    bool load_position_from_fen(const std::string& fen);
    std::string get_fen() const;

    bool white_to_move() const;
    bool is_check() const;
    std::string get_status_text() const;

    bool has_last_move() const;
    Move get_last_move() const;
};