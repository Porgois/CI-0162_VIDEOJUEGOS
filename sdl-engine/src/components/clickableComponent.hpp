#ifndef CLICKABLE_COMPONENT_HPP
#define CLICKABLE_COMPONENT_HPP

struct ClickableComponent {
    bool is_clicked;

    ClickableComponent() {
        is_clicked = false;
    }
};

#endif // CLICKABLE_COMPONENT_HPP