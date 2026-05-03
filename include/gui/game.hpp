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

    const Board &getBoard() const;

    MoveList getLegalMoves() const;
    MoveList getLegalMovesForSquare(int square) const;

    bool tryMakeMove(const Move &move);

    void resetToStart();
    bool loadPositionFromFen(const std::string &fen);
    std::string get_fen() const;

    bool whiteToMove() const;
    bool isCheck() const;
    std::string getStatusText() const;

    bool hasLastMove() const;
    Move getLastMove() const;

    bool make_ai_move(int depth);
};