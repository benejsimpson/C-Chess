#pragma once

#include <SFML/System/Vector2.hpp>

#include <algorithm>
#include <cmath>

constexpr unsigned WINDOW_WIDTH = 1000;
constexpr unsigned WINDOW_HEIGHT = 820;

struct LayoutMetrics
{
    float padding = 12.f;
    float gap = 16.f;
    float board_left = 12.f;
    float board_top = 12.f;
    float board_size = 800.f;
    float square_size = 100.f;
    float panel_left = 828.f;
    float panel_top = 20.f;
    float panel_width = 160.f;
};

inline LayoutMetrics compute_layout(sf::Vector2u window_size)
{
    const float width = static_cast<float>(window_size.x);
    const float height = static_cast<float>(window_size.y);

    LayoutMetrics layout;
    layout.padding = std::clamp(std::min(width, height) * 0.015f, 10.f, 24.f);
    layout.gap = std::clamp(layout.padding * 1.1f, 12.f, 28.f);

    layout.panel_width = std::clamp(width * 0.19f, 170.f, 300.f);

    float available_board_width = width - (layout.padding * 2.f) - layout.gap - layout.panel_width;
    if (available_board_width < 260.f)
    {
        layout.panel_width = std::max(150.f, width - (layout.padding * 2.f) - layout.gap - 260.f);
        available_board_width = width - (layout.padding * 2.f) - layout.gap - layout.panel_width;
    }

    layout.board_size = std::floor(std::min(height - (layout.padding * 2.f), available_board_width));
    layout.board_size = std::max(160.f, layout.board_size);
    layout.square_size = layout.board_size / 8.f;

    layout.board_left = layout.padding;
    layout.board_top = std::max(layout.padding, (height - layout.board_size) * 0.5f);

    layout.panel_left = layout.board_left + layout.board_size + layout.gap;
    layout.panel_top = layout.board_top + std::max(8.f, layout.padding * 0.5f);

    return layout;
}
