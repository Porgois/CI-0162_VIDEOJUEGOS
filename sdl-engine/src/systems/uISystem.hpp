#ifndef UI_SYSTEM_HPP
#define UI_SYSTEM_HPP

#include <SDL2/SDL.h>
#include <memory>
#include <string>
#include <sol/sol.hpp>

#include "../e.c.s./ecs.hpp"
#include "../events/clickEvent.hpp"
#include "../components/textComponent.hpp"
#include "../eventManager/eventManager.hpp"
#include "../components/transformComponent.hpp"
#include "../components/clickableComponent.hpp"
#include "../components/scriptComponent.hpp"

class UISystem : public System {
    private:
        sol::state* lua = nullptr;

    public:
        SDL_Color box_color = {255, 0, 0, 90}; // red semi-transparent
        SDL_Color outline_color = {255, 255, 255, 255}; // white

        UISystem() {
            requireComponent<TransformComponent>();
            requireComponent<ClickableComponent>();
        }
        
        // UISystem.hpp
        void subscribeClickEvent(std::unique_ptr<EventManager>& event_manager, sol::state& lua) {
            this->lua = &lua;
            event_manager->subscribeEvent<ClickEvent, UISystem>(this, &UISystem::onClickEvent);
        }

        void onClickEvent(ClickEvent& event) {
            for (auto entity : getSystemEntities()) {
                const auto& clickable = entity.getComponent<ClickableComponent>();
                const auto& transform = entity.getComponent<TransformComponent>();

                float box_x = transform.position.x + clickable.offset_x;
                float box_y = transform.position.y + clickable.offset_y;

                if (event.pos_x > box_x &&
                    event.pos_x < box_x + clickable.width &&
                    event.pos_y > box_y &&
                    event.pos_y < box_y + clickable.height) {

                    std::cout << "Clicked on entity #" << entity.getId() << "!" << std::endl;
                    if (entity.hasComponent<ScriptComponent>()) {
                        const auto& script = entity.getComponent<ScriptComponent>();
                        if (script.onClick != sol::nil) {
                            (*lua)["this"] = entity;
                            script.onClick();
                        }
                    }
                }
            }
        }

        void debug_draw(SDL_Renderer* renderer) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

            for (auto entity : getSystemEntities()) {
                const auto& clickable  = entity.getComponent<ClickableComponent>();
                const auto& transform  = entity.getComponent<TransformComponent>();

                float box_x = transform.position.x + clickable.offset_x;
                float box_y = transform.position.y + clickable.offset_y;

                // Box
                SDL_SetRenderDrawColor(renderer, box_color.r, box_color.g, box_color.b, box_color.a);
                SDL_Rect fill = {
                    static_cast<int>(box_x),
                    static_cast<int>(box_y),
                    clickable.width,
                    clickable.height
                };
                SDL_RenderFillRect(renderer, &fill);

                // Outline
                SDL_SetRenderDrawColor(renderer, outline_color.r, outline_color.g, outline_color.b, outline_color.a);
                SDL_RenderDrawRect(renderer, &fill);
            }

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
};

#endif // UI_SYSTEM_HPP