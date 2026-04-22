#include "gui/input.hpp"

#include "gui/constants.hpp"

#include <SFML/Window/Clipboard.hpp>

namespace
{
bool is_side_to_move_piece(const Game& game, Piece piece)
{
    return piece != Empty &&
           ((game.white_to_move() && is_white(piece)) || (!game.white_to_move() && is_black(piece)));
}

void clear_selection(GuiState& gui)
{
    gui.selected_square = -1;
    gui.selected_moves.clear();
    gui.dragging = false;
    gui.drag_from_square = -1;
    gui.drag_hover_square = -1;
    gui.dragged_piece = Empty;
}

sf::FloatRect promotion_popup_bounds(const GuiState& gui)
{
    const float square_size = static_cast<float>(SQUARE_SIZE);
    const float popup_width = square_size * 4.f;
    const float popup_height = square_size;

    int file = index_to_file(gui.pending_promotion_move.to);
    int rank = index_to_rank(gui.pending_promotion_move.to);
    int draw_file = gui.board_flipped ? 7 - file : file;
    int draw_rank = gui.board_flipped ? rank : 7 - rank;

    float x = static_cast<float>(BOARD_LEFT) + square_size * static_cast<float>(draw_file) - square_size * 1.5f;
    float y = static_cast<float>(BOARD_TOP) + square_size * static_cast<float>(draw_rank);

    if (x < static_cast<float>(BOARD_LEFT))
        x = static_cast<float>(BOARD_LEFT);
    if (x + popup_width > static_cast<float>(BOARD_LEFT + BOARD_SIZE_PX))
        x = static_cast<float>(BOARD_LEFT + BOARD_SIZE_PX) - popup_width;
    if (y + popup_height > static_cast<float>(BOARD_TOP + BOARD_SIZE_PX))
        y = static_cast<float>(BOARD_TOP + BOARD_SIZE_PX) - popup_height;

    return sf::FloatRect({x, y}, {popup_width, popup_height});
}

Piece promotion_piece_for_index(const Move& move, int index)
{
    if (is_white(move.piece))
    {
        const Piece pieces[4] = {WQ, WR, WB, WN};
        return pieces[index];
    }

    const Piece pieces[4] = {BQ, BR, BB, BN};
    return pieces[index];
}
}

bool is_mouse_over_button(const Button& button, sf::Vector2f mouse_pos)
{
    return button.bounds.contains(mouse_pos);
}

bool is_mouse_over_textbox(const TextBox& box, sf::Vector2f mouse_pos)
{
    return box.bounds.contains(mouse_pos);
}

void InputHandler::handle_event(
    const sf::Event& event,
    sf::RenderWindow& window,
    Game& game,
    GuiState& gui,
    Button& flip_button,
    Button& copy_fen_button,
    Button& load_fen_button,
    TextBox& fen_box
)
{
    if (event.is<sf::Event::Closed>())
    {
        window.close();
        return;
    }

    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Escape)
        {
            if (gui.show_promotion_popup)
            {
                gui.show_promotion_popup = false;
                return;
            }

            window.close();
            return;
        }
    }

    if (const auto* mouse = event.getIf<sf::Event::MouseMoved>())
    {
        handle_mouse_move(sf::Vector2f(static_cast<float>(mouse->position.x), static_cast<float>(mouse->position.y)), gui);
    }
    else if (const auto* press = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (press->button == sf::Mouse::Button::Left)
        {
            handle_mouse_press(
                sf::Vector2f(static_cast<float>(press->position.x), static_cast<float>(press->position.y)),
                game,
                gui,
                flip_button,
                copy_fen_button,
                load_fen_button,
                fen_box
            );
        }
    }
    else if (const auto* release = event.getIf<sf::Event::MouseButtonReleased>())
    {
        if (release->button == sf::Mouse::Button::Left)
        {
            handle_mouse_release(
                sf::Vector2f(static_cast<float>(release->position.x), static_cast<float>(release->position.y)),
                game,
                gui
            );
        }
    }

    handle_text_input(event, gui, fen_box);
}

void InputHandler::update(Game& game, GuiState& gui, float dt)
{
    (void)game;

    if (!gui.show_illegal_flash)
        return;

    gui.illegal_flash_timer -= dt;
    if (gui.illegal_flash_timer <= 0.f)
    {
        gui.show_illegal_flash = false;
        gui.illegal_flash_square = -1;
        gui.illegal_flash_timer = 0.f;
    }
}

void InputHandler::handle_mouse_press(
    sf::Vector2f mouse_pos,
    Game& game,
    GuiState& gui,
    Button& flip_button,
    Button& copy_fen_button,
    Button& load_fen_button,
    TextBox& fen_box
)
{
    handle_mouse_move(mouse_pos, gui);

    if (gui.show_promotion_popup && handle_promotion_popup_click(mouse_pos, game, gui))
        return;

    fen_box.active = is_mouse_over_textbox(fen_box, mouse_pos);
    gui.typing_fen = fen_box.active;
    if (fen_box.active)
    {
        gui.fen_input = fen_box.text;
        fen_box.cursor_pos = fen_box.text.size();
        return;
    }

    if (is_mouse_over_button(flip_button, mouse_pos))
    {
        gui.board_flipped = !gui.board_flipped;
        clear_selection(gui);
        return;
    }

    if (is_mouse_over_button(copy_fen_button, mouse_pos))
    {
        sf::Clipboard::setString(game.get_fen());
        return;
    }

    if (is_mouse_over_button(load_fen_button, mouse_pos))
    {
        const std::string fen = gui.fen_input.empty() ? fen_box.text : gui.fen_input;
        if (game.load_position_from_fen(fen))
        {
            clear_selection(gui);
            gui.show_promotion_popup = false;
        }
        else
        {
            trigger_illegal_flash(gui, 0);
        }
        return;
    }

    const int square = mouse_to_square(mouse_pos, gui.board_flipped);
    if (square == -1)
    {
        clear_selection(gui);
        return;
    }

    if (gui.selected_square != -1)
    {
        for (const Move& move : gui.selected_moves)
        {
            if (move.to == square)
            {
                try_click_move(game, gui, square);
                return;
            }
        }
    }

    const Piece piece = game.get_board().squares[square];
    if (is_side_to_move_piece(game, piece))
    {
        select_square(game, gui, square);
        gui.dragging = true;
        gui.drag_from_square = square;
        gui.dragged_piece = piece;
    }
    else
    {
        clear_selection(gui);
    }
}

void InputHandler::handle_mouse_release(sf::Vector2f mouse_pos, Game& game, GuiState& gui)
{
    if (!gui.dragging)
        return;

    gui.dragging = false;
    const int target_square = mouse_to_square(mouse_pos, gui.board_flipped);

    if (target_square == -1)
    {
        gui.drag_from_square = -1;
        gui.dragged_piece = Empty;
        return;
    }

    try_drop_move(game, gui, target_square);
}

void InputHandler::handle_mouse_move(sf::Vector2f mouse_pos, GuiState& gui)
{
    gui.drag_mouse_x = mouse_pos.x;
    gui.drag_mouse_y = mouse_pos.y;
    gui.drag_hover_square = mouse_to_square(mouse_pos, gui.board_flipped);
}

void InputHandler::handle_text_input(const sf::Event& event, GuiState& gui, TextBox& fen_box)
{
    if (!fen_box.active)
        return;

    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        if (key->control && key->code == sf::Keyboard::Key::V)
        {
            gui.fen_input = static_cast<std::string>(sf::Clipboard::getString());
        }
        else if (key->code == sf::Keyboard::Key::Backspace)
        {
            if (!gui.fen_input.empty())
                gui.fen_input.pop_back();
        }
        else if (key->code == sf::Keyboard::Key::Escape)
        {
            fen_box.active = false;
            gui.typing_fen = false;
        }

        fen_box.text = gui.fen_input;
        fen_box.cursor_pos = fen_box.text.size();
        return;
    }

    if (const auto* text = event.getIf<sf::Event::TextEntered>())
    {
        const char32_t ch = text->unicode;
        if (ch >= 32 && ch <= 126)
        {
            gui.fen_input += static_cast<char>(ch);
            fen_box.text = gui.fen_input;
            fen_box.cursor_pos = fen_box.text.size();
        }
    }
}

bool InputHandler::handle_promotion_popup_click(sf::Vector2f mouse_pos, Game& game, GuiState& gui)
{
    if (!gui.show_promotion_popup)
        return false;

    const sf::FloatRect popup = promotion_popup_bounds(gui);
    if (!popup.contains(mouse_pos))
    {
        gui.show_promotion_popup = false;
        return true;
    }

    const float square_size = static_cast<float>(SQUARE_SIZE);
    int index = static_cast<int>((mouse_pos.x - popup.position.x) / square_size);
    if (index < 0 || index > 3)
        return true;

    Move move = gui.pending_promotion_move;
    move.promotion = promotion_piece_for_index(move, index);

    if (game.try_make_move(move))
    {
        gui.show_promotion_popup = false;
        clear_selection(gui);
    }

    return true;
}

int InputHandler::mouse_to_square(sf::Vector2f mouse_pos, bool board_flipped) const
{
    if (mouse_pos.x < BOARD_LEFT || mouse_pos.y < BOARD_TOP)
        return -1;

    if (mouse_pos.x >= BOARD_LEFT + BOARD_SIZE_PX || mouse_pos.y >= BOARD_TOP + BOARD_SIZE_PX)
        return -1;

    const int draw_file = static_cast<int>((mouse_pos.x - static_cast<float>(BOARD_LEFT)) / static_cast<float>(SQUARE_SIZE));
    const int draw_rank = static_cast<int>((mouse_pos.y - static_cast<float>(BOARD_TOP)) / static_cast<float>(SQUARE_SIZE));

    const int file = board_flipped ? 7 - draw_file : draw_file;
    const int rank = board_flipped ? draw_rank : 7 - draw_rank;
    return file_rank_to_index(file, rank);
}

void InputHandler::select_square(Game& game, GuiState& gui, int square)
{
    clear_selection(gui);

    const Piece piece = game.get_board().squares[square];
    if (!is_side_to_move_piece(game, piece))
        return;

    gui.selected_square = square;
    gui.selected_moves = game.get_legal_moves_for_square(square);
    gui.drag_from_square = square;
    gui.dragged_piece = piece;
}

void InputHandler::try_click_move(Game& game, GuiState& gui, int target_square)
{
    std::vector<Move> matching_moves;
    for (const Move& move : gui.selected_moves)
    {
        if (move.to == target_square)
            matching_moves.push_back(move);
    }

    if (matching_moves.empty())
    {
        trigger_illegal_flash(gui, target_square);
        return;
    }

    if (matching_moves.front().promotion != Empty)
    {
        gui.show_promotion_popup = true;
        gui.pending_promotion_move = matching_moves.front();
        gui.dragging = false;
        return;
    }

    if (game.try_make_move(matching_moves.front()))
    {
        gui.show_promotion_popup = false;
        clear_selection(gui);
    }
}

void InputHandler::try_drop_move(Game& game, GuiState& gui, int target_square)
{
    if (target_square == gui.selected_square)
    {
        gui.drag_from_square = -1;
        gui.dragged_piece = Empty;
        return;
    }

    try_click_move(game, gui, target_square);

    gui.drag_from_square = -1;
    gui.dragged_piece = Empty;
}

void InputHandler::trigger_illegal_flash(GuiState& gui, int square)
{
    gui.show_illegal_flash = true;
    gui.illegal_flash_square = square;
    gui.illegal_flash_timer = 0.18f;
}
