#pragma once
#include <SFML/Graphics.hpp>
#include "core/board.hpp"

enum class WidgetTextureId
{
    FlipBoard,
    FlipBoardAuto
};

class TextureManager
{
private:
    sf::Texture piece_textures[12]; // indexed using piece_to_bb_ind
    sf::Texture widget_textures[2];
    bool widget_texture_loaded[2] = {false, false};

public:
    bool load_piece_textures();
    bool load_widget_textures();

    const sf::Texture& get_piece_texture(Piece piece) const;
    const sf::Texture* get_widget_texture(WidgetTextureId id) const;
};
