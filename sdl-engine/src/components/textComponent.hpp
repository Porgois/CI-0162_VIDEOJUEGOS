#ifndef TEXT_COMPONENT_HPP
#define TEXT_COMPONENT_HPP
#include <SDL2/SDL.h>
#include <string>
struct TextComponent {
    std::string text;
    std::string font_id;
    SDL_Color color;
    SDL_Color outline_color;
    int outline_thickness;
    int width;
    int height;
    bool is_ui;

    TextComponent(
        const std::string& text = "", 
        const std::string& font_id = "",
        u_char r = 0,
        u_char g = 0,
        u_char b = 0,
        u_char a = 255,
        int outline_thickness = 0,
        u_char out_r = 0,
        u_char out_g = 0,
        u_char out_b = 0,
        u_char out_a = 255,
        bool is_ui_flag = false
    ) {
        this->text = text;
        this->font_id = font_id;
        this->color = {r, g, b, a};
        this->outline_color = {out_r, out_g, out_b, out_a};
        this->outline_thickness = outline_thickness;
        this->width = 0;
        this->height = 0;
        this->is_ui = is_ui_flag;
    }
};
#endif // TEXT_COMPONENT_HPP