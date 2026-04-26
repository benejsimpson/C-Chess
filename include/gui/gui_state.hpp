#pragma once
#include <string>
#include <vector>
#include "core/move.hpp"
#include "core/board.hpp"

enum class GameMode
{
    TwoPlayer,
    WhiteAI,
    BlackAI
};

struct GuiState
{
    GameMode game_mode = GameMode::TwoPlayer;
    int search_depth = 4;
    
                                                    // Selection
    int selected_square = -1;
    std::vector<Move> selected_moves;

                                                    // Dragging
    bool dragging = false;
    int drag_from_square = -1;
    int drag_hover_square = -1;

    float drag_mouse_x = 0.0f;
    float drag_mouse_y = 0.0f;

    Piece dragged_piece = Empty;

                                                    // Board state
    bool board_flipped = false;
    bool flip_every_turn = false;

                                                    // Illegal moves
    bool show_illegal_flash = false;
    int illegal_flash_square = -1;
    float illegal_flash_timer = 0.0f;

                                                    // Promotion
    bool show_promotion_popup = false;
    Move pending_promotion_move{};

                                                    // FEN input
    std::string fen_input;
    bool typing_fen = false;
};
