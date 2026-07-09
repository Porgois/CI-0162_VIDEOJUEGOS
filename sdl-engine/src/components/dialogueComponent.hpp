#ifndef DIALOGUE_COMPONENT_HPP
#define DIALOGUE_COMPONENT_HPP

#include <cstddef>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "../e.c.s./ecs.hpp"

struct DialogueComponent {
    std::vector<std::string> lines;
    std::size_t current_line_index = 0;
    std::size_t visible_chars = 0;
    float time_since_last_char = 0.0f;
    float typing_speed = 0.035f;
    bool is_active = false;
    bool waiting_for_input = false;
    std::string sound_effect_path = "assets/soundEffects/misc/hits/hit_wall.wav";
    int wrap_at_chars = 0;
    int sound_volume = 64;
    Entity advance_indicator = Entity(-1, 0);
    glm::vec2 advance_indicator_offset = glm::vec2(16.0f, 28.0f);

    DialogueComponent() = default;
};

#endif // DIALOGUE_COMPONENT_HPP
