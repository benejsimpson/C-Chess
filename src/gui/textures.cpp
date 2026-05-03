#include "gui/textures.hpp"

#include <array>
#include <string>

bool TextureManager::loadPieceTextures()
{
    struct Entry
    {
        Piece piece;
        const char *file;
    };

    const Entry entries[] = {
        {WP, "assets/pieces/wp.png"},
        {WN, "assets/pieces/wn.png"},
        {WB, "assets/pieces/wb.png"},
        {WR, "assets/pieces/wr.png"},
        {WQ, "assets/pieces/wq.png"},
        {WK, "assets/pieces/wk.png"},
        {BP, "assets/pieces/bp.png"},
        {BN, "assets/pieces/bn.png"},
        {BB, "assets/pieces/bb.png"},
        {BR, "assets/pieces/br.png"},
        {BQ, "assets/pieces/bq.png"},
        {BK, "assets/pieces/bk.png"}};

    for (const auto &e : entries)
    {
        const int index = pieceToBitboardIndex(e.piece);
        if (index < 0 || !piece_textures[index].loadFromFile(e.file))
            return false;
    }

    return true;
}

bool TextureManager::loadWidgetTextures()
{
    struct Entry
    {
        WidgetTextureId id;
        const char *stem;
    };

    const std::array<Entry, 2> entries = {{{WidgetTextureId::FlipBoard, "assets/widgets/flip_board"},
                                           {WidgetTextureId::FlipBoardAuto, "assets/widgets/flip_board_auto"}}};

    const std::array<const char *, 5> extensions = {{".png",
                                                     ".jpg",
                                                     ".jpeg",
                                                     ".bmp",
                                                     ".tga"}};

    for (const Entry &entry : entries)
    {
        const int index = static_cast<int>(entry.id);

        for (const char *extension : extensions)
        {
            if (widget_textures[index].loadFromFile(std::string(entry.stem) + extension))
            {
                widget_texture_loaded[index] = true;
                break;
            }
        }
    }

    return true;
}

// returns the texture for a given piece
const sf::Texture &TextureManager::getPieceTexture(Piece piece) const
{
    // Convert piece to index (0–11)
    int index = pieceToBitboardIndex(piece);

    // Return the corresponding texture
    return piece_textures[index];
}

const sf::Texture *TextureManager::getWidgetTexture(WidgetTextureId id) const
{
    const int index = static_cast<int>(id);
    return widget_texture_loaded[index] ? &widget_textures[index] : nullptr;
}
