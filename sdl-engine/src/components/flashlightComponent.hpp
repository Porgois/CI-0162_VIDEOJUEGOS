#ifndef FLASHLIGHT_COMPONENT_HPP
#define FLASHLIGHT_COMPONENT_HPP

#include <string>
// Light Modes
enum class FlashlightMode {
    Full,       // circle + cone
    CircleOnly, // just the source circle (ambient glow, lantern, etc.)
    ConeOnly    // just the cone
};

struct FlashlightComponent {
    std::string cone_texture_id;
    std::string circle_texture_id;
    FlashlightMode mode = FlashlightMode::Full;

    // Cone texture dimensions
    int cone_width = 200;
    int cone_height = 180;
    int cone_end_offset = 20.0f;

    int cone_origin_x = 10;
    int cone_origin_y = cone_height / 2; // half of the cone height
    
    // Offset from entity that "holds" it
    float origin_offset_x = 15.f;
    float origin_offset_y = 15.f;
    int source_radius = 20;

    // Flicker
    float flicker_intensity = 1.0f;
    float flicker_speed = 0.1f;
    bool flicker_enabled = true;

    // Dynamic scale
    float min_scale = 0.2f;
    float max_scale = 2.0f;
    float max_length = 2.0f;
    float max_width = 1.25f;
    float reach = 250.0f; // distance at which max_scale is reached
    float max_distance = 320.0f; // distance beyond which it stops following the mouse

    
    FlashlightComponent(const std::string& cone_texture_id = "flashlight_cone", \
        const std::string& circle_texture_id = "flashlight_source", \
        int cone_width = 100, int cone_height = 90)
        : cone_texture_id(cone_texture_id),
        circle_texture_id(circle_texture_id),
        cone_width(cone_width), cone_height(cone_height),
        cone_origin_y(cone_height / 2) {}

};

#endif // FLASHLIGHT_COMPONENT_HPP