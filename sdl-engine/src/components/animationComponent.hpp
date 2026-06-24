#ifndef ANIMATION_COMPONENT_HPP
#define ANIMATION_COMPONENT_HPP

#include <SDL2/SDL.h>

struct AnimationClip {
    int row;
    int frame_count;
    int speed;
    int loop;
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

    bool isFinished() const {
        auto it = clips.find(current_animation);
        if (it == clips.end()) {
            return true;
        }

        const AnimationClip& clip = it->second;
        if (clip.loop) {
            return false;
        }

        Uint32 elapsed = SDL_GetTicks() - start_time;
        Uint32 duration = (clip.frame_count * 1000) / clip.speed;
        
        return elapsed >= duration;
    }
};

#endif // ANIMATION_COMPONENT_HPP