#pragma once
#include <SFML/Graphics.hpp>

// Forward declarations of your own types
class Game;
struct GuiState;
class TextureManager;
struct Button;
struct TextBox;

class Renderer
{
public:
    void draw(
        sf::RenderWindow& window,
        const Game& game,
        const GuiState& gui,
        const TextureManager& textures,
        const sf::Font& font,
        const Button& flip_button,
        const Button& copy_fen_button,
        const Button& load_fen_button,
        const TextBox& fen_box
    );

private:
    void draw_board(sf::RenderWindow& window, const GuiState& gui);
    void draw_last_move(sf::RenderWindow& window, const Game& game, const GuiState& gui);
    void draw_selected_square(sf::RenderWindow& window, const GuiState& gui);
    void draw_legal_moves(sf::RenderWindow& window, const GuiState& gui);
    void draw_check_highlight(sf::RenderWindow& window, const Game& game, const GuiState& gui);
    void draw_pieces(sf::RenderWindow& window, const Game& game, const GuiState& gui, const TextureManager& textures);
    void draw_dragged_piece(sf::RenderWindow& window, const GuiState& gui, const TextureManager& textures);
    void draw_side_panel(
        sf::RenderWindow& window,
        const Game& game,
        const GuiState& gui,
        const sf::Font& font,
        const Button& flip_button,
        const Button& copy_fen_button,
        const Button& load_fen_button,
        const TextBox& fen_box
    );
    void draw_promotion_popup(sf::RenderWindow& window, const GuiState& gui, const TextureManager& textures);
    void draw_illegal_flash(sf::RenderWindow& window, const GuiState& gui);

    sf::Vector2f square_to_screen(int square, bool board_flipped) const;
};