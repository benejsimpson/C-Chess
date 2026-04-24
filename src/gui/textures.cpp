#include "gui/textures.hpp"

bool TextureManager::load_piece_textures()
{
    struct Entry
    {
        Piece piece;
        const char* file;
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
        {BK, "assets/pieces/bk.png"}
    };

    for (const auto& e : entries)
    {
        const int index = piece_to_bb_ind(e.piece);
        if (index < 0 || !piece_textures[index].loadFromFile(e.file))
            return false;
    }

    return true;
}


// returns the texture for a given piece
const sf::Texture& TextureManager::get_piece_texture(Piece piece) const
{
    // Convert piece to index (0–11)
    int index = piece_to_bb_ind(piece);

    // Return the corresponding texture
    return piece_textures[index];
}
