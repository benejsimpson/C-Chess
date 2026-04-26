#pragma once
#include <SFML/Graphics.hpp>
#include "game.hpp"
#include "gui_state.hpp"
#include "renderer.hpp"
#include "input.hpp"
#include "textures.hpp"
#include "widgets.hpp"
#include "constants.hpp"

class ChessGui
{
private:
    sf::RenderWindow window;
    sf::Font font;

    Game game;
    GuiState gui;

    TextureManager textures;
    Renderer renderer;
    InputHandler input;

    Button flip_button;
    Button auto_flip_button;
    Button copy_fen_button;
    Button load_fen_button;
    Button two_player_button;
    Button white_ai_button;
    Button black_ai_button;
    TextBox fen_box;


public:
    ChessGui();
    bool init();
    void run();

private:
    void setup_widgets();
    void update_layout();
};
