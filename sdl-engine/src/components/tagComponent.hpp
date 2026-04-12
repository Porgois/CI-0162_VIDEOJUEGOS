#ifndef TAG_COMPONENT_HPP
#define TAG_COMPONENT_HPP

#include <string>
#include <SDL2/SDL.h>

struct TagComponent {
    std::string tag;

    TagComponent(const std::string& tag_name = "") {
        this->tag = tag_name;
    }

};

#endif // TAG_COMPONENT_HPP