#ifndef SCENE_LOADER_HPP
#define SCENE_LOADER_HPP

#include <SDL2/SDL.h>
#include <memory>
#include <sol/sol.hpp>
#include <string>

#include "../assetManager/assetManager.hpp"
#include "../controllerManager/controllerManager.hpp"
#include "../e.c.s./ecs.hpp"

class SceneLoader {
    private:
        void loadSprites(SDL_Renderer* renderer, \
            const sol::table& sprites, std::unique_ptr<AssetManager>& asset_manager);

        void loadKeys(const sol::table& keys, \
            std::unique_ptr<ControllerManager>& controller_manager);

        void loadEntities(sol::state& lua, const sol::table& entities, \
            std::unique_ptr<Registry>& registry);
        
        void loadAnimation(Entity& entity, const sol::table& components);
        
        void loadCursor(Entity& entity, const sol::table& components);

        void loadBoxCollider(Entity& entity, const sol::table& components);

        void loadCollider(Entity& entity, const sol::table& components);

        void loadCircleCollider(Entity& entity, const sol::table& components);

        void loadRigidbody(Entity& entity, const sol::table& components);

        void loadScript(sol::state& lua, Entity& entity, const sol::table& components);

        void loadSprite(Entity& entity, const sol::table& components);

        void loadTransform(Entity& entity, const sol::table& components);

    public:
        SceneLoader();

        ~SceneLoader();

        void loadScene(const std::string& scene_path, \
            sol::state& lua, \
            std::unique_ptr<AssetManager>& asset_manager, \
            std::unique_ptr<ControllerManager>& controller_manager,
            std::unique_ptr<Registry>& registry, \
            SDL_Renderer* renderer);
};

#endif // SCENE_LOADER_HPP