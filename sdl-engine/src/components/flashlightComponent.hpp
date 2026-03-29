#ifndef FLASHLIGHT_COMPONENT_HPP
#define FLASHLIGHT_COMPONENT_HPP

#include <string>

struct FlashlightComponent {
    std::string cone_texture_id;
    std::string circle_texture_id;

    // Cone texture dimensions
    int cone_width = 150;
    int cone_height = 90;
    int cone_end_offset = 60.0f;

    int cone_origin_x = 10;
    int cone_origin_y = cone_height / 2; // half of the cone height
    
    // Offset from entity that "holds" it
    float origin_offset_x = 45.f;
    float origin_offset_y = 45.f;

    int source_radius = 65;

    bool is_on = true; // can be turned off

    // Flicker
    float flicker_intensity = 1.0f;
    float flicker_speed = 0.1f;
    bool flicker_enabled = true;

    // Dynamic scale
    float min_scale = 0.0f;
    float max_scale = 1.0f;
    float reach = 450.0f; // distance at which max_scale is reached

    FlashlightComponent(const std::string& cone_texture_id = "flashlight_cone", \
        const std::string& circle_texture_id = "flashlight-source", \
        int cone_width = 150, int cone_height = 90)
        : cone_texture_id(cone_texture_id),
        circle_texture_id(circle_texture_id),
        cone_width(cone_width), cone_height(cone_height),
        cone_origin_y(cone_height / 2) {}

};

#endif // FLASHLIGHT_COMPONENT_HPP