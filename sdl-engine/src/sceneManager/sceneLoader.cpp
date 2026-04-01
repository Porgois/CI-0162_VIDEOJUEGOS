#include <iostream> 
#include <glm/glm.hpp>

#include "./sceneLoader.hpp"

#include "../components/scriptComponent.hpp"
#include "../components/cursorComponent.hpp"
#include "../components/animationComponent.hpp"
#include "../components/spriteComponent.hpp"
#include "../components/transformComponent.hpp"
#include "../components/rigidBodyComponent.hpp"
#include "../components/flashlightComponent.hpp"
#include "../components/circleColliderComponent.hpp"
#include "../components/boxColliderComponent.hpp"
#include "../components/colliderComponent.hpp"

SceneLoader::SceneLoader() {
    std::cout << "[SCENE LOADER] Executes constructor!" << std::endl;
}

SceneLoader::~SceneLoader() {
    std::cout << "[SCENE LOADER] Executes destructor!" << std::endl;
}

void SceneLoader::loadScene(const std::string& scene_path, \
    sol::state& lua, \
    std::unique_ptr<AssetManager>& asset_manager, \
    std::unique_ptr<ControllerManager>& controller_manager,
    std::unique_ptr<Registry>& registry, \
    SDL_Renderer* renderer) {

    sol::load_result script_result = lua.load_file(scene_path);

    if (!script_result.valid()) {
        sol::error err = script_result;
        std::string err_message = err.what();
        std::cout << "[SCENE LOADER] " << err_message << std::endl;
        return;
    }

    lua.script_file(scene_path);
    sol::table scene = lua["scene"];
    sol::table sprites = scene["sprites"];
    loadSprites(renderer, sprites, asset_manager);

    sol::table keys = scene["keys"];
    loadKeys(keys, controller_manager);

    sol::table entities = scene["entities"];
    loadEntities(lua, entities, registry);
}

void SceneLoader::loadSprites(SDL_Renderer* renderer, \
    const sol::table& sprites, std::unique_ptr<AssetManager>& asset_manager) {
    int index = 0;
    while (true) {
        sol::optional<sol::table> has_sprite = sprites[index];

        if (has_sprite == sol::nullopt) { // nothing
            break;
        }

        sol::table sprite = sprites[index];

        std::string assetId = sprite["assetId"];
        std::string file_path = sprite["file_path"];

        asset_manager->addTexture(renderer, assetId, file_path);
        index++;
    } 
}

void SceneLoader::loadKeys(const sol::table& keys, \
    std::unique_ptr<ControllerManager>& controller_manager) {
    int index = 0;
    while (true) {
        sol::optional<sol::table> has_key = keys[index];

        if (has_key == sol::nullopt) { // nothing
            break;
        }

        sol::table key = keys[index];

        std::string action_name = key["name"];
        int key_code = key["key"];

        controller_manager->addActionKey(action_name, key_code);
        index++;
    }
}

void SceneLoader::loadAnimation(Entity& entity, const sol::table& components) {
    sol::optional<sol::table> has_animation = components["animation"];

    if (has_animation != sol::nullopt) {
        entity.addComponent<AnimationComponent>();
        auto& anim = entity.getComponent<AnimationComponent>();

        sol::optional<sol::table> has_clips = components["animation"]["clips"];

        if (has_clips != sol::nullopt) {
            sol::table clips = components["animation"]["clips"];

            for (auto& [key, value] : clips) {
                std::string clip_name = key.as<std::string>();
                sol::table clip = value.as<sol::table>();

                anim.clips[clip_name] = \
                    {clip["row"], clip["num_frames"], clip["animation_speed"], clip["loop"].get<bool>()};
            }
        }
      
    }
}

void SceneLoader::loadCursor(Entity& entity, const sol::table& components) {
    sol::optional<sol::table> has_cursor = components["cursor"];

    if (has_cursor != sol::nullopt) {
        entity.addComponent<CursorComponent>();
    }
}

void SceneLoader::loadCollider(Entity& entity, const sol::table& components) {
    sol::optional<sol::table> has_collider = components["collider"];

    if (has_collider != sol::nullopt) {
        entity.addComponent<ColliderComponent>();
    }
}

//int width = 0, int height = 0, glm::vec2 offset = glm::vec2(0.0, 0.0)
void SceneLoader::loadBoxCollider(Entity& entity, const sol::table& components) {
    sol::optional<sol::table> has_box_collider = components["box_collider"];

    if (has_box_collider != sol::nullopt) {
        entity.addComponent<BoxColliderComponent> (
            components["box_collider"]["width"],
            components["box_collider"]["height"],
            glm::vec2(
                components["box_collider"]["offset"]["x"],
                components["box_collider"]["offset"]["y"]
            )
        );
    }
}

void SceneLoader::loadCircleCollider(Entity& entity, const sol::table& components) {
    sol::optional<sol::table> has_circle_collider = components["circle_collider"];

    if (has_circle_collider != sol::nullopt) {
        entity.addComponent<CircleColliderComponent> (
            components["circle_collider"]["radius"],
            components["circle_collider"]["width"],
            components["circle_collider"]["height"]
        );
    }
}

void SceneLoader::loadRigidbody(Entity& entity, const sol::table& components) {
    sol::optional<sol::table> has_rigidbody = components["rigidbody"];

    if (has_rigidbody != sol::nullopt) {
        entity.addComponent<RigidBodyComponent>( 
            glm::vec2(
                components["rigidbody"]["velocity"]["x"],
                components["rigidbody"]["velocity"]["y"]
            )
        );
    }
}

void SceneLoader::loadScript(sol::state& lua, Entity& entity, const sol::table& components) {
    sol::optional<sol::table> has_script = components["script"];

    if (has_script != sol::nullopt) {
        lua["update"] = sol::nil;

        std::string script_path = components["script"]["path"];
        lua.script_file(script_path);

        sol::optional<sol::function> has_update = lua["update"];
        sol::function update = sol::nil;

        if (has_update != sol::nullopt) {
            update = lua["update"];
        }

        entity.addComponent<ScriptComponent>(update);
    }
}

void SceneLoader::loadSprite(Entity& entity, const sol::table& components) {
    sol::optional<sol::table> has_sprite = components["sprite"];

    if (has_sprite != sol::nullopt) {
        entity.addComponent<SpriteComponent>( 
            components["sprite"]["assetId"],
            components["sprite"]["width"],
            components["sprite"]["height"],
            components["sprite"]["src_rect"]["x"],
            components["sprite"]["src_rect"]["y"],
            components["sprite"]["flip"]
        );
    }
}

void SceneLoader::loadTransform(Entity& entity, const sol::table& components) {
    sol::optional<sol::table> has_transform = components["transform"];

    if (has_transform != sol::nullopt) {
        entity.addComponent<TransformComponent>(
            glm::vec2( // position
                components["transform"]["position"]["x"],
                components["transform"]["position"]["y"]
            ),
            glm::vec2( // scale
                components["transform"]["scale"]["x"],
                components["transform"]["scale"]["y"]
            ),
            components["transform"]["rotation"]
        );
    }
}


void SceneLoader::loadEntities(sol::state& lua, const sol::table& entities, \
    std::unique_ptr<Registry>& registry) {
    int index = 0;
    while (true) {
        sol::optional<sol::table> has_entity = entities[index];

        if (has_entity == sol::nullopt) { // nothing
            break;
        }

        sol::table entity = entities[index];
        
        Entity new_entity = registry->createEntity("");

        sol::optional<sol::table> has_components = entity["components"];

        if (has_components != sol::nullopt) {
            sol::table components = entity["components"];

            // Box Collider
            loadAnimation(new_entity, components);
            std::cout << "[SCENE LOADER] Loading cursor..." << std::endl;
            loadCursor(new_entity, components);
            std::cout << "[SCENE LOADER] Loading circle collider..." << std::endl;
            loadCircleCollider(new_entity, components);
            std::cout << "[SCENE LOADER] Loading rigidbody..." << std::endl;
            loadRigidbody(new_entity, components);
            std::cout << "[SCENE LOADER] Loading sprite..." << std::endl;
            loadSprite(new_entity, components);
            std::cout << "[SCENE LOADER] Loading transform..." << std::endl;
            loadTransform(new_entity, components);
            std::cout << "[SCENE LOADER] Loading script..." << std::endl;
            loadScript(lua, new_entity, components);
            std::cout << "[SCENE LOADER] All components loaded!" << std::endl;
        }

        index++;
    }
}

