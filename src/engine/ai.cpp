// ai.cpp
#include "engine/ai.hpp"

bool is_ai_turn(const Board& board, const GuiState& state)
{
    if (state.game_mode == GameMode::WhiteAI && board.white_to_move)
        return true;

    if (state.game_mode == GameMode::BlackAI && !board.white_to_move)
        return true;

    return false;
}