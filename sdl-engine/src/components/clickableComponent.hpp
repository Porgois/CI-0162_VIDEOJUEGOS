#ifndef CLICKABLE_COMPONENT_HPP
#define CLICKABLE_COMPONENT_HPP

struct ClickableComponent {
    int width;
    int height;
    int offset_x;
    int offset_y;
    
    ClickableComponent(int width = 0, int height = 0, int offset_x = 0, int offset_y = 0)
        : width(width), height(height), offset_x(offset_x), offset_y(offset_y) {}
};

#endif // CLICKABLE_COMPONENT_HPP