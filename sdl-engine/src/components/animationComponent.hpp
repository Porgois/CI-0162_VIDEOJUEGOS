#ifndef ANIMATION_COMPONENT_HPP
#define ANIMATION_COMPONENT_HPP

#include <SDL2/SDL.h>

struct AnimationClip {
    int row;
    int frame_count;
    int speed;
    int loops;
};

struct AnimationComponent {
    std::unordered_map<std::string, AnimationClip> clips;
    std::string current_animation = "";
    int current_frame = 0;
    int start_time = 0;

    void play(const std::string& animation_name) {
        if (current_animation != animation_name) {
            current_animation = animation_name;
            current_frame = 0;
            start_time = SDL_GetTicks();
        }
    }

    AnimationClip& currentClip() {
        auto it = clips.find(current_animation);
        if (it == clips.end()) {
            static AnimationClip empty = {0, 1, 1, true};
            return empty;
        }
        return it->second;
    }
};

#endif // ANIMATION_COMPONENT_HPP