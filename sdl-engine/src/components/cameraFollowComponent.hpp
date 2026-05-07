#ifndef CAMERA_FOLLOW_COMPONENT_HPP
#define CAMERA_FOLLOW_COMPONENT_HPP

#include <glm/glm.hpp>

#include "../components/transformComponent.hpp"

struct CameraFollowComponent {
    bool is_active = true;
    CameraFollowComponent() {}

};

#endif // CAMERA_FOLLOW_COMPONENT_HPP