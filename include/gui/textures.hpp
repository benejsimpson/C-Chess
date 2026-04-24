#pragma once
#include <SFML/Graphics.hpp>
#include "core/board.hpp"

class TextureManager
{
private:
    sf::Texture piece_textures[12]; // indexed using piece_to_bb_ind

public:
    bool load_piece_textures();

    const sf::Texture& get_piece_texture(Piece piece) const;
};
