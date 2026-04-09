#ifndef UI_SYSTEM_HPP
#define UI_SYSTEM_HPP

#include <SDL2/SDL.h>
#include <memory>
#include <string>

#include "../e.c.s./ecs.hpp"
#include "../events/clickEvent.hpp"
#include "../components/textComponent.hpp"
#include "../eventManager/eventManager.hpp"
#include "../components/transformComponent.hpp"
#include "../components/clickableComponent.hpp"
#include "../components/scriptComponent.hpp"

class UISystem : public System {
    public:
        UISystem() {
            requireComponent<TransformComponent>();
            requireComponent<TextComponent>();
            requireComponent<ClickableComponent>();
        }
        
        void subscribeClickEvent(std::unique_ptr<EventManager>& event_manager) {
            event_manager->subscribeEvent<ClickEvent, UISystem>(this, &UISystem::onClickEvent);
        };

        void onClickEvent(ClickEvent& event) {
            for (auto entity : getSystemEntities()) {
                const auto& text = entity.getComponent<TextComponent>();
                const auto& transform = entity.getComponent<TransformComponent>();

                if (transform.position.x < event.pos_x && event.pos_x < transform.position.x + text.width && \
                    transform.position.y < event.pos_y && event.pos_y < transform.position.y + text.height) {
                    
                    std::cout << "Clicked on entity #" << entity.getId() << "!" << std::endl;

                    if (entity.hasComponent<ScriptComponent>()) {
                        const auto& script = entity.getComponent<ScriptComponent>();
                        if (script.onClick != sol::nil) {
                            script.onClick();
                        }
                    }
                }
            }
        }

};

#endif // UI_SYSTEM_HPP

// VIDEO #9