#pragma once
#include <SFML/Graphics.hpp>
#include "board.hpp"

class TextureManager
{
private:
    sf::Texture piece_textures[12]; // indexed using piece_to_bitboard_index

public:
    bool load_piece_textures();

    const sf::Texture& get_piece_texture(Piece piece) const;
};

bool TextureManager::load_piece_textures()
{
    struct Entry
    {
        Piece piece;
        const char* file;
    };

    Entry entries[] = {
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
        int index = piece_to_bitboard_index(e.piece);

        if (!piece_textures[index].loadFromFile(e.file))
            return false;
    }

    return true;
}