#include "gui/renderer.hpp"

#include "gui/gui_state.hpp"
#include "gui/game.hpp"
#include "gui/constants.hpp"
#include "gui/textures.hpp"
#include "gui/widgets.hpp"
#include "core/board.hpp"
#include "core/move.hpp"

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace
{
int find_checked_king_square(const Board& board, bool white_king)
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
// Rendering helpers
// --------------------------------------------------

std::string wrap_text(const sf::Font& font, const std::string& text, unsigned int character_size, float max_width)
{
    if (text.empty())
        return {};

    auto fits_width = [&](const std::string& candidate) -> bool
    {
        sf::Text measure(font, candidate, character_size);
        return measure.getLocalBounds().size.x <= max_width;
    };

    auto break_long_word = [&](const std::string& word) -> std::vector<std::string>
    {
        std::vector<std::string> parts;
        std::string current;

        for (char ch : word)
        {
            const std::string candidate = current + ch;
            if (!current.empty() && !fits_width(candidate))
            {
                parts.push_back(current);
                current = std::string(1, ch);
            }
            else
            {
                current = candidate;
            }
        }

        if (!current.empty())
            parts.push_back(current);

        return parts;
    };

    std::stringstream input(text);
    std::string word;
    std::string current_line;
    std::string wrapped;

    while (input >> word)
    {
        std::vector<std::string> word_parts = fits_width(word) ? std::vector<std::string>{word} : break_long_word(word);
        for (const std::string& part : word_parts)
        {
            const std::string candidate = current_line.empty() ? part : current_line + " " + part;
            if (!current_line.empty() && !fits_width(candidate))
            {
                if (!wrapped.empty())
                    wrapped += '\n';
                wrapped += current_line;
                current_line = part;
            }
            else
            {
                current_line = candidate;
            }
        }
    }

    if (!current_line.empty())
    {
        if (!wrapped.empty())
            wrapped += '\n';
        wrapped += current_line;
    }

    return wrapped;
}

static void draw_button(
    sf::RenderWindow& window,
    const Button& button,
    const sf::Font& font
)
{
    sf::RectangleShape rect;
    rect.setPosition(sf::Vector2f(button.bounds.position.x, button.bounds.position.y));
    rect.setSize(sf::Vector2f(button.bounds.size.x, button.bounds.size.y));
    rect.setFillColor(button.hovered ? sf::Color(76, 76, 76) : sf::Color(55, 55, 55));
    rect.setOutlineThickness(2.f);
    rect.setOutlineColor(button.active ? sf::Color(234, 197, 110) : sf::Color(180, 180, 180));
    window.draw(rect);

    if (button.icon_texture != nullptr)
    {
        sf::Sprite sprite(*button.icon_texture);
        const sf::Vector2u tex_size = button.icon_texture->getSize();
        if (tex_size.x > 0 && tex_size.y > 0)
        {
            const float padding = std::max(6.f, std::min(button.bounds.size.x, button.bounds.size.y) * 0.14f);
            const float target_size = std::min(button.bounds.size.x, button.bounds.size.y) - padding * 2.f;
            sprite.setScale(sf::Vector2f(
                target_size / static_cast<float>(tex_size.x),
                target_size / static_cast<float>(tex_size.y)
            ));
            sprite.setPosition(sf::Vector2f(
                button.bounds.position.x + (button.bounds.size.x - target_size) * 0.5f,
                button.bounds.position.y + (button.bounds.size.y - target_size) * 0.5f
            ));
            window.draw(sprite);
            return;
        }
    }

    const unsigned int character_size = static_cast<unsigned int>(std::clamp(button.bounds.size.y * 0.42f, 16.f, 24.f));
    sf::Text text(font, button.label, character_size);
    text.setFillColor(sf::Color::White);

    const sf::FloatRect text_bounds = text.getLocalBounds();
    text.setPosition(sf::Vector2f(
        button.bounds.position.x + (button.bounds.size.x - text_bounds.size.x) * 0.5f,
        button.bounds.position.y + (button.bounds.size.y - text_bounds.size.y) * 0.5f - text_bounds.position.y
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
    rect.setPosition(sf::Vector2f(box.bounds.position.x, box.bounds.position.y));
    rect.setSize(sf::Vector2f(box.bounds.size.x, box.bounds.size.y));
    rect.setFillColor(sf::Color(35, 35, 35));
    rect.setOutlineThickness(2.f);
    rect.setOutlineColor(box.active ? sf::Color::Yellow : sf::Color(150, 150, 150));
    window.draw(rect);

    const unsigned int character_size = static_cast<unsigned int>(std::clamp(box.bounds.size.y * 0.18f, 16.f, 22.f));
    sf::Text text(font, wrap_text(font, box.text, character_size, box.bounds.size.x - 16.f), character_size);
    text.setFillColor(sf::Color::White);
    text.setPosition(sf::Vector2f(
        box.bounds.position.x + 8.f,
        box.bounds.position.y + 8.f
    ));
    window.draw(text);
}

static void draw_tooltip(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& text,
    sf::Vector2f anchor
)
{
    const unsigned int character_size = 14;
    sf::Text tooltip_text(font, text, character_size);
    tooltip_text.setFillColor(sf::Color::White);

    const sf::FloatRect text_bounds = tooltip_text.getLocalBounds();
    const sf::Vector2f padding(10.f, 6.f);
    const sf::Vector2u window_size = window.getSize();

    sf::Vector2f box_size(
        text_bounds.size.x + padding.x * 2.f,
        text_bounds.size.y + padding.y * 2.f + 4.f
    );

    sf::Vector2f position(anchor.x, anchor.y - box_size.y - 8.f);
    if (position.x + box_size.x > static_cast<float>(window_size.x) - 8.f)
        position.x = static_cast<float>(window_size.x) - box_size.x - 8.f;
    if (position.x < 8.f)
        position.x = 8.f;
    if (position.y < 8.f)
        position.y = anchor.y + 8.f;

    sf::RectangleShape box;
    box.setPosition(position);
    box.setSize(box_size);
    box.setFillColor(sf::Color(24, 24, 24, 235));
    box.setOutlineThickness(1.f);
    box.setOutlineColor(sf::Color(210, 210, 210));
    window.draw(box);

    tooltip_text.setPosition(sf::Vector2f(
        position.x + padding.x,
        position.y + padding.y - text_bounds.position.y
    ));
    window.draw(tooltip_text);
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
    const Button& auto_flip_button,
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
        auto_flip_button,
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
    const LayoutMetrics layout = compute_layout(window.getSize());
    const float square_size = layout.square_size;
    const float board_left  = layout.board_left;
    const float board_top   = layout.board_top;

    const sf::Color light_square(240, 217, 181);
    const sf::Color dark_square(181, 136, 99);

    for (int square = 0; square < 64; square++)
    {
        int file = index_to_file(square);
        int rank = index_to_rank(square);

        sf::Vector2f pos = square_to_screen(square, gui.board_flipped, square_size);

        sf::RectangleShape tile;
        tile.setPosition(sf::Vector2f(
            board_left + pos.x,
            board_top + pos.y
        ));
        tile.setSize(sf::Vector2f(square_size, square_size));

        bool light = ((file + rank) % 2 == 1);
        tile.setFillColor(light ? light_square : dark_square);

        window.draw(tile);
    }
}

void Renderer::draw_last_move(sf::RenderWindow& window, const Game& game, const GuiState& gui)
{
    if (!game.has_last_move())
        return;

    Move last = game.get_last_move();

    const LayoutMetrics layout = compute_layout(window.getSize());
    const float square_size = layout.square_size;
    const float board_left  = layout.board_left;
    const float board_top   = layout.board_top;

    sf::Color highlight(246, 246, 105, 90);

    int squares[2] = { last.from, last.to };

    for (int sq : squares)
    {
        sf::Vector2f pos = square_to_screen(sq, gui.board_flipped, square_size);

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

    const LayoutMetrics layout = compute_layout(window.getSize());
    const float square_size = layout.square_size;
    const float board_left  = layout.board_left;
    const float board_top   = layout.board_top;

    sf::Vector2f pos = square_to_screen(gui.selected_square, gui.board_flipped, square_size);

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
    const LayoutMetrics layout = compute_layout(window.getSize());
    const float square_size = layout.square_size;
    const float board_left  = layout.board_left;
    const float board_top   = layout.board_top;

    for (const Move& move : gui.selected_moves)
    {
        sf::Vector2f pos = square_to_screen(move.to, gui.board_flipped, square_size);

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
    const int king_square = find_checked_king_square(board, game.white_to_move());
    if (king_square == -1)
        return;

    const LayoutMetrics layout = compute_layout(window.getSize());
    const float square_size = layout.square_size;
    const float board_left  = layout.board_left;
    const float board_top   = layout.board_top;

    sf::Vector2f pos = square_to_screen(king_square, gui.board_flipped, square_size);

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
    const Board& board = game.get_board();

    const LayoutMetrics layout = compute_layout(window.getSize());
    const float square_size = layout.square_size;
    const float board_left  = layout.board_left;
    const float board_top   = layout.board_top;

    for (int square = 0; square < 64; square++)
    {
        Piece piece = board.squares[square];

        if (piece == Empty)
            continue;

        // if dragging, do not draw the original square version of the dragged piece.
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

        sf::Vector2f pos = square_to_screen(square, gui.board_flipped, square_size);
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

    const LayoutMetrics layout = compute_layout(window.getSize());
    const float square_size = layout.square_size;

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
    const Button& auto_flip_button,
    const Button& copy_fen_button,
    const Button& load_fen_button,
    const TextBox& fen_box
)
{
    const LayoutMetrics layout = compute_layout(window.getSize());
    const sf::Vector2f mouse_pos(gui.drag_mouse_x, gui.drag_mouse_y);

    Button flip_display = flip_button;
    flip_display.hovered = is_mouse_over_button(flip_display, mouse_pos);

    Button auto_flip_display = auto_flip_button;
    auto_flip_display.hovered = is_mouse_over_button(auto_flip_display, mouse_pos);
    auto_flip_display.active = gui.flip_every_turn;

    Button copy_fen_display = copy_fen_button;
    copy_fen_display.hovered = is_mouse_over_button(copy_fen_display, mouse_pos);

    Button load_fen_display = load_fen_button;
    load_fen_display.hovered = is_mouse_over_button(load_fen_display, mouse_pos);

    const float section_gap = std::max(18.f, layout.padding);
    draw_button(window, flip_display, font);
    draw_button(window, auto_flip_display, font);
    draw_button(window, copy_fen_display, font);
    draw_button(window, load_fen_display, font);
    draw_textbox(window, fen_box, font);

    const unsigned int label_size = static_cast<unsigned int>(std::clamp(layout.board_size * 0.025f, 17.f, 24.f));
    const unsigned int body_size = static_cast<unsigned int>(std::clamp(layout.board_size * 0.024f, 16.f, 22.f));

    sf::Text fen_label(font, "FEN Input", label_size);
    fen_label.setPosition(sf::Vector2f(layout.panel_left, fen_box.bounds.position.y - label_size - 8.f));
    fen_label.setFillColor(sf::Color::White);
    window.draw(fen_label);

    sf::Text status_label(font, "Status", label_size);
    status_label.setPosition(sf::Vector2f(layout.panel_left, load_fen_button.bounds.position.y + load_fen_button.bounds.size.y + section_gap));
    status_label.setFillColor(sf::Color::White);
    window.draw(status_label);

    sf::Text status_text(font, game.get_status_text(), body_size);
    status_text.setPosition(sf::Vector2f(layout.panel_left, status_label.getPosition().y + label_size + 8.f));
    status_text.setFillColor(sf::Color(220, 220, 220));
    window.draw(status_text);

    if (flip_display.hovered)
        draw_tooltip(window, font, flip_display.tooltip, sf::Vector2f(flip_display.bounds.position.x, flip_display.bounds.position.y));
    else if (auto_flip_display.hovered)
        draw_tooltip(window, font, auto_flip_display.tooltip, sf::Vector2f(auto_flip_display.bounds.position.x, auto_flip_display.bounds.position.y));
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

    const LayoutMetrics layout = compute_layout(window.getSize());
    const float square_size = layout.square_size;
    const float popup_width = square_size * 4.f;
    const float popup_height = square_size;

    const sf::Vector2f target_pos = square_to_screen(gui.pending_promotion_move.to, gui.board_flipped, square_size);
    float x = layout.board_left + target_pos.x - square_size * 1.5f;
    float y = layout.board_top + target_pos.y;

    if (x < layout.board_left)
        x = layout.board_left;
    if (x + popup_width > layout.board_left + layout.board_size)
        x = layout.board_left + layout.board_size - popup_width;
    if (y + popup_height > layout.board_top + layout.board_size)
        y = layout.board_top + layout.board_size - popup_height;

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

    const LayoutMetrics layout = compute_layout(window.getSize());
    const float square_size = layout.square_size;
    const float board_left  = layout.board_left;
    const float board_top   = layout.board_top;

    sf::Vector2f pos = square_to_screen(gui.illegal_flash_square, gui.board_flipped, square_size);

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

sf::Vector2f Renderer::square_to_screen(int square, bool board_flipped, float square_size) const
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
        static_cast<float>(draw_file) * square_size,
        static_cast<float>(draw_rank) * square_size
    );
}
