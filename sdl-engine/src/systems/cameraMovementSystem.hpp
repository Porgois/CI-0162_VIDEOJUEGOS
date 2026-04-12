#ifndef CAMERA_MOVEMENT_SYSTEM_HPP
#define CAMERA_MOVEMENT_SYSTEM_HPP

#include <SDL2/SDL.h>

#include "../e.c.s./ecs.hpp"
#include "../game/game.hpp"
#include "../components/transformComponent.hpp"
#include "../components/cameraFollowComponent.hpp"

class CameraMovementSystem : public System {
    public:
        CameraMovementSystem() {
            requireComponent<TransformComponent>();
            requireComponent<CameraFollowComponent>();
        }

        void update(SDL_Rect& camera) {
            for (auto& entity : getSystemEntities()) {
                const auto& transform = entity.getComponent<TransformComponent>();

                // Width
                if (transform.position.x + (camera.w / 2) 
                    <static_cast<float>(Game::getInstance().map_width)) {
                    camera.x = static_cast<int>(transform.position.x) - (camera.w / 2);
                }
                
                // Height
                if (transform.position.y + (camera.h / 2) 
                    <static_cast<float>(Game::getInstance().map_height)) {
                    camera.y = static_cast<int>(transform.position.y) - (camera.h / 2);
                }

                // Adjust camera
                camera.x = camera.x < 0 ? 0 : camera.x;
                camera.y = camera.y < 0 ? 0 : camera.y; 
            }
        }
};

#endif // CAMERA_MOVEMENT_SYSTEM_HPP