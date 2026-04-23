#ifndef SCENE_LOADER_HPP
#define SCENE_LOADER_HPP

#include <SDL2/SDL.h>
#include <tinyxml2/tinyxml2.h>
#include <memory>
#include <sol/sol.hpp>
#include <string>

#include "../assetManager/assetManager.hpp"
#include "../controllerManager/controllerManager.hpp"
#include "../e.c.s./ecs.hpp"

class SceneLoader {
    private:
        void loadFonts(const sol::table& fonts, \
            std::unique_ptr<AssetManager>& asset_manager);

        void loadSprites(SDL_Renderer* renderer, \
            const sol::table& sprites, std::unique_ptr<AssetManager>& asset_manager);

        void loadKeys(const sol::table& keys, \
            std::unique_ptr<ControllerManager>& controller_manager);

        void loadButtons(const sol::table& buttons, \
            std::unique_ptr<ControllerManager>& controller_manager);

        void loadEntities(sol::state& lua, const sol::table& entities, \
            std::unique_ptr<Registry>& registry);
        
        void loadTag(Entity& entity, const sol::table& components);

        void loadAnimation(Entity& entity, const sol::table& components);
        
        void loadCursor(Entity& entity, const sol::table& components);

        void loadBoxCollider(Entity& entity, const sol::table& components);

        void loadCollider(Entity& entity, const sol::table& components);

        void loadCircleCollider(Entity& entity, const sol::table& components);

        void loadRigidbody(Entity& entity, const sol::table& components);

        void loadScript(sol::state& lua, Entity& entity, const sol::table& components);

        void loadSprite(Entity& entity, const sol::table& components);

        void loadTransform(Entity& entity, const sol::table& components);

        void loadText(Entity& entity, const sol::table& components);

        void loadClickable(Entity& entity, const sol::table& components);

        void loadCameraFollow(Entity& entity, const sol::table& components);

        void loadColliders(std::unique_ptr<Registry>& registry, tinyxml2::XMLElement* object_group);

        void loadFlashlight(Entity& entity, const sol::table& components);
        
        SDL_RendererFlip getFlip(bool flip_d, bool flip_h, bool flip_v);

        double getAngle(bool flip_d, bool flip_h, bool flip_v);

        void renderTile(SDL_Renderer* renderer, SDL_Texture* tileset_texture, uint32_t tile_id,
        bool flip_h, bool flip_v, bool flip_d,
        int tile_number, int tile_width, int tile_height,
        int map_width, int columns);

        void processTile(SDL_Renderer* renderer, SDL_Texture* tileset_texture, \
        const std::string& number_str, \
        int tile_number, int tile_width, int tile_height, \
        int map_width, int columns);

        SDL_Texture* bakeLayer(SDL_Renderer* renderer, const char* data, SDL_Texture* tileset_texture, \
        int tile_width, int tile_height, \
        int map_width, int map_height, \
        int columns);

        void loadMap(SDL_Renderer* renderer, const sol::table map, std::unique_ptr<Registry>& registry, \
            std::unique_ptr<AssetManager>& asset_manager);

        void loadLayer(SDL_Renderer* renderer, std::unique_ptr<Registry>& registry, \
            tinyxml2::XMLElement* layer, int tile_width, int tile_height, \
            int map_width, int map_height, \
            const std::string& tileset, int columns, int z_index, \
            std::unique_ptr<AssetManager>& asset_manager);

        void loadButton();

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