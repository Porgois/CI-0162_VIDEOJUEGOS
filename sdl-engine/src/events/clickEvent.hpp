#ifndef CLICK_EVENT_HPP
#define CLICK_EVENT_HPP

#include "../e.c.s./ecs.hpp"
#include "../eventManager/event.hpp"

class ClickEvent : public Event {
    public:
        int button_code;
        int pos_x;
        int pos_y;
    
        ClickEvent(int button_code = 0, int pos_x = 0, int pos_y = 0) {\
            this->button_code = button_code;
            this->pos_x = pos_x;
            this->pos_y = pos_y;
        }
};

#endif // CLICK_EVENT_HPP