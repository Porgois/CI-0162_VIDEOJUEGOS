#ifndef DIALOGUE_SYSTEM_HPP
#define DIALOGUE_SYSTEM_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>
#include <vector>

#include "../e.c.s./ecs.hpp"
#include "../components/dialogueComponent.hpp"
#include "../components/textComponent.hpp"
#include "../components/transformComponent.hpp"
#include "../components/spriteComponent.hpp"
#include "../eventManager/eventManager.hpp"
#include "../events/clickEvent.hpp"
#include "../game/game.hpp"
#include "../sceneManager/sceneLoader.hpp"

class DialogueSystem : public System {
    public:
        DialogueSystem() {
            requireComponent<DialogueComponent>();
            requireComponent<TextComponent>();
            requireComponent<TransformComponent>();
        }

        std::string wrapText(const std::string& raw_text, int wrap_at_chars) {
            if (wrap_at_chars <= 0) {
                return raw_text;
            }

            std::string wrapped;
            int count = 0;
            for (size_t i = 0; i < raw_text.size(); ++i) {
                char c = raw_text[i];
                wrapped += c;

                if (c == '\n') {
                    count = 0;
                    continue;
                }

                count++;
                if (count >= wrap_at_chars && i + 1 < raw_text.size() && raw_text[i + 1] != '\n') {
                    wrapped += '\n';
                    count = 0;
                }
            }

            return wrapped;
        }

        void subscribeClickEvent(std::unique_ptr<EventManager>& event_manager) {
            event_manager->subscribeEvent<ClickEvent, DialogueSystem>(this, &DialogueSystem::onClickEvent);
        }

        void clearAdvanceIndicator(Entity entity) {
            if (!entity.hasComponent<DialogueComponent>()) {
                return;
            }

            auto& dialogue = entity.getComponent<DialogueComponent>();
            if (dialogue.advance_indicator.getId() != -1 && Game::getInstance().registry->isEntityValid(dialogue.advance_indicator)) {
                Game::getInstance().registry->destroyEntity(dialogue.advance_indicator);
            }
            dialogue.advance_indicator = Entity(-1, 0);
        }

        void spawnAdvanceIndicator(Entity entity) {
            if (!entity.hasComponent<DialogueComponent>() || !entity.hasComponent<TransformComponent>()) {
                return;
            }

            auto& dialogue = entity.getComponent<DialogueComponent>();
            if (dialogue.advance_indicator.getId() != -1 && Game::getInstance().registry->isEntityValid(dialogue.advance_indicator)) {
                return;
            }

            sol::state& lua = Game::getInstance().lua;
            sol::load_result script = lua.load_file("./assets/scripts/entities/e_dialogue_advance_indicator.lua");
            if (!script.valid()) {
                return;
            }

            sol::protected_function_result result = script();
            if (!result.valid()) {
                return;
            }

            sol::table entity_def = result;
            Entity indicator = SceneLoader::createEntity(lua, entity_def, Game::getInstance().registry, Game::getInstance().named_entities);

            if (!indicator.hasComponent<TransformComponent>()) {
                indicator.addComponent<TransformComponent>(glm::vec2(0.0f), glm::vec2(1.0f), 0.0);
            }

            auto& indicator_transform = indicator.getComponent<TransformComponent>();
            dialogue.advance_indicator_offset = indicator_transform.position;
            dialogue.advance_indicator = indicator;
            updateIndicatorPosition(entity);
        }

        void getTextDimensions(const TextComponent& text, int& out_width, int& out_height) {
            out_width = 0;
            out_height = 0;

            TTF_Font* font = Game::getInstance().asset_manager->getFont(text.font_id);
            if (!font) {
                return;
            }

            std::vector<std::string> lines;
            size_t start = 0;
            size_t pos = 0;
            while ((pos = text.text.find('\n', start)) != std::string::npos) {
                lines.push_back(text.text.substr(start, pos - start));
                start = pos + 1;
            }
            lines.push_back(text.text.substr(start));

            int total_height = 0;
            for (const std::string& line : lines) {
                int line_width = 0;
                int line_height = 0;
                if (line.empty()) {
                    line_width = 0;
                    line_height = TTF_FontLineSkip(font);
                } else {
                    TTF_SizeText(font, line.c_str(), &line_width, &line_height);
                }
                out_width = std::max(out_width, line_width);
                total_height += line_height;
            }
            out_height = total_height;
        }

        void updateIndicatorPosition(Entity entity) {
            if (!entity.hasComponent<DialogueComponent>() || !entity.hasComponent<TransformComponent>()) {
                return;
            }

            auto& dialogue = entity.getComponent<DialogueComponent>();
            if (dialogue.advance_indicator.getId() == -1 || !Game::getInstance().registry->isEntityValid(dialogue.advance_indicator)) {
                return;
            }

            if (!dialogue.advance_indicator.hasComponent<TransformComponent>()) {
                dialogue.advance_indicator.addComponent<TransformComponent>(glm::vec2(0.0f), glm::vec2(1.0f), 0.0);
            }

            auto& indicator_transform = dialogue.advance_indicator.getComponent<TransformComponent>();

            if (entity.hasComponent<TextComponent>()) {
                auto& text = entity.getComponent<TextComponent>();
                int text_width = 0;
                int text_height = 0;
                getTextDimensions(text, text_width, text_height);
                text_width = std::max(1, text_width);
                text_height = std::max(1, text_height);

                float screen_center_x = 400.0f;
                float text_bottom_y = 300.0f + (text_height / 2.0f);

                if (dialogue.advance_indicator.hasComponent<SpriteComponent>()) {
                    auto& indicator_sprite = dialogue.advance_indicator.getComponent<SpriteComponent>();
                    float indicator_width = indicator_sprite.width * indicator_transform.scale.x;
                    float indicator_height = indicator_sprite.height * indicator_transform.scale.y;
                    float pivot_x = (indicator_sprite.pivot.x != 0 || indicator_sprite.pivot.y != 0) ? indicator_width / 2.0f : 0.0f;
                    float pivot_y = (indicator_sprite.pivot.x != 0 || indicator_sprite.pivot.y != 0) ? indicator_height / 2.0f : 0.0f;

                    indicator_transform.position = glm::vec2(
                        screen_center_x - pivot_x,
                        text_bottom_y + 8.0f - pivot_y
                    );
                } else {
                    indicator_transform.position = glm::vec2(
                        screen_center_x,
                        text_bottom_y + 8.0f
                    );
                }
                return;
            }

            auto& dialog_transform = entity.getComponent<TransformComponent>();
            indicator_transform.position = dialog_transform.position + dialogue.advance_indicator_offset;
        }

        void update(float delta_time) {
            for (auto entity : getSystemEntities()) {
                auto& dialogue = entity.getComponent<DialogueComponent>();
                auto& text = entity.getComponent<TextComponent>();

                if (!dialogue.is_active || dialogue.lines.empty()) {
                    continue;
                }

                const std::string& current_line = dialogue.lines[dialogue.current_line_index];
                if (dialogue.waiting_for_input) {
                    if (dialogue.advance_indicator.getId() == -1 || !Game::getInstance().registry->isEntityValid(dialogue.advance_indicator)) {
                        spawnAdvanceIndicator(entity);
                    }
                    updateIndicatorPosition(entity);
                    continue;
                }

                dialogue.time_since_last_char += delta_time;
                while (dialogue.time_since_last_char >= dialogue.typing_speed && dialogue.visible_chars < current_line.size()) {
                    dialogue.time_since_last_char -= dialogue.typing_speed;
                    ++dialogue.visible_chars;
                    if (!dialogue.sound_effect_path.empty()) {
                        Game::getInstance().audio_manager->playSound(dialogue.sound_effect_path, 0, dialogue.sound_volume);
                    }
                }

                if (dialogue.visible_chars >= current_line.size()) {
                    dialogue.waiting_for_input = true;
                    if (dialogue.advance_indicator.getId() == -1 || !Game::getInstance().registry->isEntityValid(dialogue.advance_indicator)) {
                        spawnAdvanceIndicator(entity);
                    }
                }

                text.text = wrapText(current_line.substr(0, dialogue.visible_chars), dialogue.wrap_at_chars);

                if (dialogue.is_active && dialogue.waiting_for_input) {
                    updateIndicatorPosition(entity);
                }
            }
        }

        void startDialogue(Entity entity, const std::vector<std::string>& lines, const std::string& sound_effect_path = "", int sound_volume = 64, float typing_speed = 0.035f, int wrap_at_chars = 0) {
            if (!entity.hasComponent<DialogueComponent>() || !entity.hasComponent<TextComponent>()) {
                return;
            }

            auto& dialogue = entity.getComponent<DialogueComponent>();
            dialogue.lines = lines;
            dialogue.current_line_index = 0;
            dialogue.visible_chars = 0;
            dialogue.time_since_last_char = 0.0f;
            dialogue.typing_speed = typing_speed;
            dialogue.wrap_at_chars = wrap_at_chars;
            dialogue.is_active = true;
            dialogue.waiting_for_input = false;
            dialogue.sound_effect_path = sound_effect_path;
            dialogue.sound_volume = sound_volume;
            clearAdvanceIndicator(entity);
            entity.getComponent<TextComponent>().text = "";
        }

        void advanceDialogue(Entity entity) {
            if (!entity.hasComponent<DialogueComponent>()) {
                return;
            }

            auto& dialogue = entity.getComponent<DialogueComponent>();
            if (!dialogue.is_active || dialogue.lines.empty()) {
                return;
            }

            if (!dialogue.waiting_for_input) {
                return;
            }

            ++dialogue.current_line_index;
            if (dialogue.current_line_index >= dialogue.lines.size()) {
                dialogue.is_active = false;
                dialogue.waiting_for_input = false;
                clearAdvanceIndicator(entity);
                entity.getComponent<TextComponent>().text = "";

                if (Game::getInstance().scene_manager) {
                    const std::string next_scene = Game::getInstance().scene_manager->getNextSceneInList();
                    if (!next_scene.empty()) {
                        Game::getInstance().scene_manager->setNextScene(next_scene);
                        Game::getInstance().requestSceneTransition();
                    }
                }
                return;
            }

            clearAdvanceIndicator(entity);

            dialogue.visible_chars = 0;
            dialogue.time_since_last_char = 0.0f;
            dialogue.waiting_for_input = false;
            entity.getComponent<TextComponent>().text = "";
        }

        void onClickEvent(ClickEvent& event) {
            (void)event;
            for (auto entity : getSystemEntities()) {
                auto& dialogue = entity.getComponent<DialogueComponent>();
                if (dialogue.is_active && dialogue.waiting_for_input) {
                    advanceDialogue(entity);
                }
            }
        }
};

#endif // DIALOGUE_SYSTEM_HPP
