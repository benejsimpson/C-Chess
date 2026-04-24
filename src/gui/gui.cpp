#include "gui/gui.hpp"

#include <array>
#include <algorithm>

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
        sf::Style::Default
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

    textures.load_widget_textures();

    game.reset_to_start();
    gui = GuiState{};
    sync_board_orientation(game, gui);
    update_layout();
    return true;
}

void ChessGui::run()
{
    sf::Clock clock;

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                window.setView(sf::View(sf::FloatRect(
                    {0.f, 0.f},
                    {static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)}
                )));
                update_layout();
            }

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
        fen_box.text = gui.fen_input;

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
    flip_button.label = "Flip";
    flip_button.tooltip = "Flip board";

    auto_flip_button.label = "Auto";
    auto_flip_button.tooltip = "Flip board every turn automatically";

    copy_fen_button.label = "Copy FEN";
    load_fen_button.label = "Load FEN";
    fen_box.text.clear();
    fen_box.active = false;
    fen_box.cursor_pos = 0;
}

void ChessGui::update_layout()
{
    const LayoutMetrics layout = compute_layout(window.getSize());
    const float button_gap = std::max(10.f, layout.padding * 0.8f);
    const float icon_size = std::clamp(layout.panel_width * 0.28f, 48.f, 72.f);
    const float full_button_width = layout.panel_width;
    const float button_height = std::clamp(layout.board_size * 0.055f, 40.f, 52.f);

    flip_button.bounds = sf::FloatRect(
        {layout.panel_left, layout.panel_top},
        {icon_size, icon_size}
    );
    flip_button.icon_texture = textures.get_widget_texture(WidgetTextureId::FlipBoard);

    auto_flip_button.bounds = sf::FloatRect(
        {layout.panel_left + icon_size + button_gap, layout.panel_top},
        {icon_size, icon_size}
    );
    auto_flip_button.icon_texture = textures.get_widget_texture(WidgetTextureId::FlipBoardAuto);

    const float text_button_top = layout.panel_top + icon_size + std::max(14.f, layout.padding);

    copy_fen_button.bounds = sf::FloatRect(
        {layout.panel_left, text_button_top},
        {full_button_width, button_height}
    );

    fen_box.bounds = sf::FloatRect(
        {layout.panel_left, copy_fen_button.bounds.position.y + button_height + std::max(12.f, layout.padding * 0.8f)},
        {full_button_width, button_height * 3.f}
    );

    load_fen_button.bounds = sf::FloatRect(
        {layout.panel_left, fen_box.bounds.position.y + fen_box.bounds.size.y + std::max(12.f, layout.padding * 0.8f)},
        {full_button_width, button_height}
    );
}
