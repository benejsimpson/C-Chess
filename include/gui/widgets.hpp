#pragma once
#include <SFML/Graphics.hpp>
#include <string>

struct Button
{
    sf::FloatRect bounds;
    std::string label;
    std::string tooltip;
    const sf::Texture* icon_texture = nullptr;
    bool hovered = false;
    bool active = false;
};

struct TextBox
{
    sf::FloatRect bounds;
    std::string text;
    bool active = false;
    size_t cursor_pos = 0;
};

bool is_mouse_over_button(const Button& button, sf::Vector2f mouse_pos);
bool is_mouse_over_textbox(const TextBox& box, sf::Vector2f mouse_pos);
