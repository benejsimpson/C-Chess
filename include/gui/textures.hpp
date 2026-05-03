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
    sf::Texture piece_textures[12]; // indexed using pieceToBitboardIndex
    sf::Texture widget_textures[2];
    bool widget_texture_loaded[2] = {false, false};

public:
    bool loadPieceTextures();
    bool loadWidgetTextures();

    const sf::Texture &getPieceTexture(Piece piece) const;
    const sf::Texture *getWidgetTexture(WidgetTextureId id) const;
};
