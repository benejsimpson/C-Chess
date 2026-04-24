#pragma once
#include <SFML/Graphics.hpp>
#include "game.hpp"
#include "gui_state.hpp"
#include "widgets.hpp"

class InputHandler
{
public:
    void handle_event(
        const sf::Event& event,
        sf::RenderWindow& window,
        Game& game,
        GuiState& gui,
        Button& flip_button,
        Button& auto_flip_button,
        Button& copy_fen_button,
        Button& load_fen_button,
        TextBox& fen_box
    );

    void update(Game& game, GuiState& gui, float dt);

private:
    void handle_mouse_press(
        sf::Vector2f mouse_pos,
        Game& game,
        GuiState& gui,
        Button& flip_button,
        Button& auto_flip_button,
        Button& copy_fen_button,
        Button& load_fen_button,
        TextBox& fen_box
    );

    void handle_mouse_release(sf::Vector2f mouse_pos, Game& game, GuiState& gui);
    void handle_mouse_move(sf::Vector2f mouse_pos, GuiState& gui);
    void handle_text_input(const sf::Event& event, GuiState& gui, TextBox& fen_box);

    bool handle_promotion_popup_click(sf::Vector2f mouse_pos, Game& game, GuiState& gui);

    int mouse_to_square(sf::Vector2f mouse_pos, bool board_flipped) const;

    void select_square(Game& game, GuiState& gui, int square);
    void try_click_move(Game& game, GuiState& gui, int target_square);
    void try_drop_move(Game& game, GuiState& gui, int target_square);

    void trigger_illegal_flash(GuiState& gui, int square);
};
