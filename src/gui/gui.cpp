#include "gui/gui.hpp"

#include <array>

namespace
{
const std::array<const char*, 3> kFontCandidates = {
    "assets/DejaVuSans.ttf",
    "C:/Windows/Fonts/segoeui.ttf",
    "C:/Windows/Fonts/arial.ttf"
};

void sync_board_orientation(const Game& game, GuiState& gui)
{
    if (!gui.flip_every_turn)
        return;

    gui.board_flipped = !game.white_to_move();
}
}

ChessGui::ChessGui()
{
    setup_widgets();
}

bool ChessGui::init()
{
    window.create(
        sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}),
        "Chess++ - Benjamin Simpson",
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setFramerateLimit(60);

    bool font_loaded = false;
    for (const char* path : kFontCandidates)
    {
        if (font.openFromFile(path))
        {
            font_loaded = true;
            break;
        }
    }

    if (!font_loaded)
        return false;

    if (!textures.load_piece_textures())
        return false;

    game.reset_to_start();
    gui = GuiState{};
    sync_board_orientation(game, gui);
    fen_box.text = game.get_fen();
    gui.fen_input = fen_box.text;
    return true;
}

void ChessGui::run()
{
    sf::Clock clock;

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            input.handle_event(
                *event,
                window,
                game,
                gui,
                flip_button,
                auto_flip_button,
                copy_fen_button,
                load_fen_button,
                fen_box
            );
        }

        const float dt = clock.restart().asSeconds();
        input.update(game, gui, dt);
        sync_board_orientation(game, gui);

        if (!gui.typing_fen)
        {
            fen_box.text = game.get_fen();
            gui.fen_input = fen_box.text;
        }
        else
        {
            fen_box.text = gui.fen_input;
        }

        renderer.draw(
            window,
            game,
            gui,
            textures,
            font,
            flip_button,
            auto_flip_button,
            copy_fen_button,
            load_fen_button,
            fen_box
        );
    }
}

void ChessGui::setup_widgets()
{
    flip_button.bounds = sf::FloatRect({static_cast<float>(PANEL_LEFT), 40.f}, {150.f, 42.f});
    flip_button.label = "Flip Board";

    auto_flip_button.bounds = sf::FloatRect({static_cast<float>(PANEL_LEFT), 96.f}, {150.f, 42.f});
    auto_flip_button.label = "Flip Every Turn";

    copy_fen_button.bounds = sf::FloatRect({static_cast<float>(PANEL_LEFT), 152.f}, {150.f, 42.f});
    copy_fen_button.label = "Copy FEN";

    load_fen_button.bounds = sf::FloatRect({static_cast<float>(PANEL_LEFT), 266.f}, {150.f, 42.f});
    load_fen_button.label = "Load FEN";

    fen_box.bounds = sf::FloatRect({static_cast<float>(PANEL_LEFT), 208.f}, {160.f, 48.f});
    fen_box.text.clear();
    fen_box.active = false;
    fen_box.cursor_pos = 0;
}
