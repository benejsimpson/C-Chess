#include "gui/renderer.hpp"

#include "gui/gui_state.hpp"
#include "gui/game.hpp"
#include "gui/constants.hpp"
#include "gui/textures.hpp"
#include "gui/widgets.hpp"
#include "core/board.hpp"
#include "core/move.hpp"

#include <SFML/Graphics.hpp>
#include <string>

namespace
{
int find_king_square(const Board& board, bool white_king)
{
    const Piece king = white_king ? WK : BK;

    for (int square = 0; square < 64; ++square)
    {
        if (board.squares[square] == king)
            return square;
    }

    return -1;
}
}

// --------------------------------------------------
// Small local helpers
// Only used inside this file
// --------------------------------------------------

static void draw_button(
    sf::RenderWindow& window,
    const Button& button,
    const sf::Font& font
)
{
    sf::RectangleShape rect;
    rect.setPosition(
        sf::Vector2f(button.bounds.position.x, button.bounds.position.y)
    );
    rect.setSize(
        sf::Vector2f(button.bounds.size.x, button.bounds.size.y)
    );
    rect.setFillColor(sf::Color(60, 60, 60));
    rect.setOutlineThickness(2.f);
    rect.setOutlineColor(sf::Color(180, 180, 180));
    window.draw(rect);

    sf::Text text(font, button.label, 18);
    text.setFillColor(sf::Color::White);

    sf::FloatRect text_bounds = text.getLocalBounds();
    text.setPosition(sf::Vector2f(
        button.bounds.position.x + (button.bounds.size.x  - text_bounds.size.x) / 2.f,
        button.bounds.position.y  + (button.bounds.size.y - text_bounds.size.y) / 2.f - 6.f
    ));

    window.draw(text);
}

static void draw_textbox(
    sf::RenderWindow& window,
    const TextBox& box,
    const sf::Font& font
)
{
    sf::RectangleShape rect;
    rect.setPosition(
        sf::Vector2f(box.bounds.position.x, box.bounds.position.y)
    );
    rect.setSize(
        sf::Vector2f(box.bounds.size.x, box.bounds.size.y)
    );
    rect.setFillColor(sf::Color(35, 35, 35));
    rect.setOutlineThickness(2.f);
    rect.setOutlineColor(box.active ? sf::Color::Yellow : sf::Color(150, 150, 150));
    window.draw(rect);

    sf::Text text(font, box.text, 18);
    text.setFillColor(sf::Color::White);
    text.setPosition(sf::Vector2f(
        box.bounds.position.x + 8.f,
        box.bounds.position.y + 8.f
    ));
    window.draw(text);
}

// --------------------------------------------------
// Main draw function
// --------------------------------------------------

void Renderer::draw(
    sf::RenderWindow& window,
    const Game& game,
    const GuiState& gui,
    const TextureManager& textures,
    const sf::Font& font,
    const Button& flip_button,
    const Button& copy_fen_button,
    const Button& load_fen_button,
    const TextBox& fen_box
)
{
    window.clear(sf::Color(25, 25, 25));

    draw_board(window, gui);
    draw_last_move(window, game, gui);
    draw_selected_square(window, gui);
    draw_legal_moves(window, gui);
    draw_check_highlight(window, game, gui);
    draw_pieces(window, game, gui, textures);
    draw_dragged_piece(window, gui, textures);
    draw_side_panel(
        window,
        game,
        gui,
        font,
        flip_button,
        copy_fen_button,
        load_fen_button,
        fen_box
    );
    draw_promotion_popup(window, gui, textures);
    draw_illegal_flash(window, gui);

    window.display();
}

// --------------------------------------------------
// Board drawing
// --------------------------------------------------

void Renderer::draw_board(sf::RenderWindow& window, const GuiState& gui)
{
    (void)gui;

    const float square_size = static_cast<float>(SQUARE_SIZE);
    const float board_left  = static_cast<float>(BOARD_LEFT);
    const float board_top   = static_cast<float>(BOARD_TOP);

    const sf::Color light_square(240, 217, 181);
    const sf::Color dark_square(181, 136, 99);

    for (int square = 0; square < 64; square++)
    {
        int file = index_to_file(square);
        int rank = index_to_rank(square);

        sf::Vector2f pos = square_to_screen(square, gui.board_flipped);

        sf::RectangleShape tile;
        tile.setPosition(sf::Vector2f(
            board_left + pos.x,
            board_top + pos.y
        ));
        tile.setSize(sf::Vector2f(square_size, square_size));

        bool light = ((file + rank) % 2 == 0);
        tile.setFillColor(light ? light_square : dark_square);

        window.draw(tile);
    }
}

void Renderer::draw_last_move(sf::RenderWindow& window, const Game& game, const GuiState& gui)
{
    if (!game.has_last_move())
        return;

    Move last = game.get_last_move();

    const float square_size = static_cast<float>(SQUARE_SIZE);
    const float board_left  = static_cast<float>(BOARD_LEFT);
    const float board_top   = static_cast<float>(BOARD_TOP);

    sf::Color highlight(246, 246, 105, 90);

    int squares[2] = { last.from, last.to };

    for (int sq : squares)
    {
        sf::Vector2f pos = square_to_screen(sq, gui.board_flipped);

        sf::RectangleShape rect;
        rect.setPosition(sf::Vector2f(board_left + pos.x, board_top + pos.y));
        rect.setSize(sf::Vector2f(square_size, square_size));
        rect.setFillColor(highlight);

        window.draw(rect);
    }
}

void Renderer::draw_selected_square(sf::RenderWindow& window, const GuiState& gui)
{
    if (gui.selected_square == -1)
        return;

    const float square_size = static_cast<float>(SQUARE_SIZE);
    const float board_left  = static_cast<float>(BOARD_LEFT);
    const float board_top   = static_cast<float>(BOARD_TOP);

    sf::Vector2f pos = square_to_screen(gui.selected_square, gui.board_flipped);

    sf::RectangleShape rect;
    rect.setPosition(sf::Vector2f(board_left + pos.x, board_top + pos.y));
    rect.setSize(sf::Vector2f(square_size, square_size));
    rect.setFillColor(sf::Color(80, 170, 255, 80));
    rect.setOutlineThickness(3.f);
    rect.setOutlineColor(sf::Color(80, 170, 255));

    window.draw(rect);
}

void Renderer::draw_legal_moves(sf::RenderWindow& window, const GuiState& gui)
{
    const float square_size = static_cast<float>(SQUARE_SIZE);
    const float board_left  = static_cast<float>(BOARD_LEFT);
    const float board_top   = static_cast<float>(BOARD_TOP);

    for (const Move& move : gui.selected_moves)
    {
        sf::Vector2f pos = square_to_screen(move.to, gui.board_flipped);

        sf::CircleShape dot(square_size * 0.12f);
        dot.setFillColor(sf::Color(30, 30, 30, 140));
        dot.setPosition(sf::Vector2f(
            board_left + pos.x + square_size * 0.5f - dot.getRadius(),
            board_top + pos.y + square_size * 0.5f - dot.getRadius()
        ));

        window.draw(dot);
    }
}

void Renderer::draw_check_highlight(sf::RenderWindow& window, const Game& game, const GuiState& gui)
{
    if (!game.is_check())
        return;

    const Board& board = game.get_board();
    const int king_square = find_king_square(board, game.white_to_move());
    if (king_square == -1)
        return;

    const float square_size = static_cast<float>(SQUARE_SIZE);
    const float board_left  = static_cast<float>(BOARD_LEFT);
    const float board_top   = static_cast<float>(BOARD_TOP);

    sf::Vector2f pos = square_to_screen(king_square, gui.board_flipped);

    sf::RectangleShape rect;
    rect.setPosition(sf::Vector2f(board_left + pos.x, board_top + pos.y));
    rect.setSize(sf::Vector2f(square_size, square_size));
    rect.setFillColor(sf::Color(255, 0, 0, 90));

    window.draw(rect);
}

// --------------------------------------------------
// Piece drawing
// --------------------------------------------------

void Renderer::draw_pieces(
    sf::RenderWindow& window,
    const Game& game,
    const GuiState& gui,
    const TextureManager& textures
)
{
    // TODO:
    // Match this with your Game class.
    // Expected:
    // const Board& board = game.get_board();

    const Board& board = game.get_board();

    const float square_size = static_cast<float>(SQUARE_SIZE);
    const float board_left  = static_cast<float>(BOARD_LEFT);
    const float board_top   = static_cast<float>(BOARD_TOP);

    for (int square = 0; square < 64; square++)
    {
        Piece piece = board.squares[square];

        if (piece == Empty)
            continue;

        // If dragging, do not draw the original square version
        // of the dragged piece.
        if (gui.dragging && square == gui.drag_from_square)
            continue;

        const sf::Texture& texture = textures.get_piece_texture(piece);

        sf::Sprite sprite(texture);

        sf::Vector2u tex_size = texture.getSize();
        if (tex_size.x == 0 || tex_size.y == 0)
            continue;

        sprite.setScale(sf::Vector2f(
            square_size / static_cast<float>(tex_size.x),
            square_size / static_cast<float>(tex_size.y)
        ));

        sf::Vector2f pos = square_to_screen(square, gui.board_flipped);
        sprite.setPosition(sf::Vector2f(board_left + pos.x, board_top + pos.y));

        window.draw(sprite);
    }
}

void Renderer::draw_dragged_piece(
    sf::RenderWindow& window,
    const GuiState& gui,
    const TextureManager& textures
)
{
    if (!gui.dragging)
        return;

    if (gui.dragged_piece == Empty)
        return;

    const sf::Texture& texture = textures.get_piece_texture(gui.dragged_piece);
    sf::Sprite sprite(texture);

    sf::Vector2u tex_size = texture.getSize();
    if (tex_size.x == 0 || tex_size.y == 0)
        return;

    const float square_size = static_cast<float>(SQUARE_SIZE);

    sprite.setScale(sf::Vector2f(
        square_size / static_cast<float>(tex_size.x),
        square_size / static_cast<float>(tex_size.y)
    ));

    sprite.setPosition(sf::Vector2f(
        gui.drag_mouse_x - square_size * 0.5f,
        gui.drag_mouse_y - square_size * 0.5f
    ));

    window.draw(sprite);
}

// --------------------------------------------------
// Side panel
// --------------------------------------------------

void Renderer::draw_side_panel(
    sf::RenderWindow& window,
    const Game& game,
    const GuiState& gui,
    const sf::Font& font,
    const Button& flip_button,
    const Button& copy_fen_button,
    const Button& load_fen_button,
    const TextBox& fen_box
)
{
    (void)gui;

    draw_button(window, flip_button, font);
    draw_button(window, copy_fen_button, font);
    draw_button(window, load_fen_button, font);
    draw_textbox(window, fen_box, font);

    sf::Text status_label(font, "Status", 18);
    status_label.setPosition(sf::Vector2f(static_cast<float>(PANEL_LEFT), 270.f));
    status_label.setFillColor(sf::Color::White);
    window.draw(status_label);

    sf::Text status_text(font, game.get_status_text(), 18);
    status_text.setPosition(sf::Vector2f(static_cast<float>(PANEL_LEFT), 296.f));
    status_text.setFillColor(sf::Color(220, 220, 220));
    window.draw(status_text);

    sf::Text fen_label(font, "FEN", 18);
    fen_label.setPosition(sf::Vector2f(static_cast<float>(PANEL_LEFT), 124.f));
    fen_label.setFillColor(sf::Color::White);
    window.draw(fen_label);
}

// --------------------------------------------------
// Promotion popup
// --------------------------------------------------

void Renderer::draw_promotion_popup(
    sf::RenderWindow& window,
    const GuiState& gui,
    const TextureManager& textures
)
{
    if (!gui.show_promotion_popup)
        return;

    const float square_size = static_cast<float>(SQUARE_SIZE);
    const float popup_width = square_size * 4.f;
    const float popup_height = square_size;

    const sf::Vector2f target_pos = square_to_screen(gui.pending_promotion_move.to, gui.board_flipped);
    float x = static_cast<float>(BOARD_LEFT) + target_pos.x - square_size * 1.5f;
    float y = static_cast<float>(BOARD_TOP) + target_pos.y;

    if (x < static_cast<float>(BOARD_LEFT))
        x = static_cast<float>(BOARD_LEFT);
    if (x + popup_width > static_cast<float>(BOARD_LEFT + BOARD_SIZE_PX))
        x = static_cast<float>(BOARD_LEFT + BOARD_SIZE_PX) - popup_width;
    if (y + popup_height > static_cast<float>(BOARD_TOP + BOARD_SIZE_PX))
        y = static_cast<float>(BOARD_TOP + BOARD_SIZE_PX) - popup_height;

    sf::RectangleShape bg;
    bg.setPosition(sf::Vector2f(x, y));
    bg.setSize(sf::Vector2f(popup_width, popup_height));
    bg.setFillColor(sf::Color(40, 40, 40, 240));
    bg.setOutlineThickness(2.f);
    bg.setOutlineColor(sf::Color::White);
    window.draw(bg);

    Piece options[4];
    if (is_white(gui.pending_promotion_move.piece))
    {
        options[0] = WQ;
        options[1] = WR;
        options[2] = WB;
        options[3] = WN;
    }
    else
    {
        options[0] = BQ;
        options[1] = BR;
        options[2] = BB;
        options[3] = BN;
    }

    for (int i = 0; i < 4; i++)
    {
        const sf::Texture& texture = textures.get_piece_texture(options[i]);
        sf::Sprite sprite(texture);

        sf::Vector2u tex_size = texture.getSize();
        if (tex_size.x == 0 || tex_size.y == 0)
            continue;

        sprite.setScale(sf::Vector2f(
            square_size / static_cast<float>(tex_size.x),
            square_size / static_cast<float>(tex_size.y)
        ));

        sprite.setPosition(sf::Vector2f(
            x + i * square_size,
            y
        ));

        window.draw(sprite);
    }
}

// --------------------------------------------------
// Illegal move flash
// --------------------------------------------------

void Renderer::draw_illegal_flash(sf::RenderWindow& window, const GuiState& gui)
{
    if (!gui.show_illegal_flash || gui.illegal_flash_square == -1)
        return;

    const float square_size = static_cast<float>(SQUARE_SIZE);
    const float board_left  = static_cast<float>(BOARD_LEFT);
    const float board_top   = static_cast<float>(BOARD_TOP);

    sf::Vector2f pos = square_to_screen(gui.illegal_flash_square, gui.board_flipped);

    sf::RectangleShape overlay;
    overlay.setPosition(sf::Vector2f(board_left + pos.x, board_top + pos.y));
    overlay.setSize(sf::Vector2f(square_size, square_size));
    overlay.setFillColor(sf::Color(255, 0, 0, 120));

    window.draw(overlay);
}

// --------------------------------------------------
// Square -> screen helper
// Returns board-relative position
// --------------------------------------------------

sf::Vector2f Renderer::square_to_screen(int square, bool board_flipped) const
{
    int file = index_to_file(square);
    int rank = index_to_rank(square);

    int draw_file = file;
    int draw_rank = 7 - rank;

    if (board_flipped)
    {
        draw_file = 7 - file;
        draw_rank = rank;
    }

    // This returns position relative to top-left of the board.
    // board_left and board_top are added by the calling function.
    return sf::Vector2f(
        static_cast<float>(draw_file) * static_cast<float>(SQUARE_SIZE),
        static_cast<float>(draw_rank) * static_cast<float>(SQUARE_SIZE)
    );
}
