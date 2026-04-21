#include <iostream> 
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <sstream>

#include "./sceneLoader.hpp"

#include "../game/game.hpp"
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
#include "../components/textComponent.hpp"
#include "../components/clickableComponent.hpp"
#include "../components/cameraFollowComponent.hpp"
#include "../components/tagComponent.hpp"
#include "../components/tileMapComponent.hpp"
#include "../components/layerComponent.hpp"

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

    sol::table fonts = scene["fonts"];
    loadFonts(fonts, asset_manager);

    sol::table buttons = scene["buttons"];
    loadButtons(buttons, controller_manager);

    sol::table keys = scene["keys"];
    loadKeys(keys, controller_manager);

    sol::table entities = scene["entities"];
    loadEntities(lua, entities, registry);

    sol::table maps = scene["maps"];
    loadMap(renderer, maps, registry, asset_manager);
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
        std::string asset_id = sprite["asset_id"];
        std::string file_path = sprite["file_path"];

        asset_manager->addTexture(renderer, asset_id, file_path);
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

void SceneLoader::loadTag(Entity& entity, const sol::table& components) {
    sol::optional<sol::table> has_tag = components["tag"];

    if (has_tag != sol::nullopt) {
        std::string tag_name = components["tag"]["tag"];
        entity.addComponent<TagComponent>(tag_name);
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

void SceneLoader::loadClickable(Entity& entity, const sol::table& components) {
    sol::optional<sol::table> has_clickable = components["clickable"];

    if (has_clickable != sol::nullopt) {
        entity.addComponent<ClickableComponent>();
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
        lua["on_click"] = sol::nil;
        lua["update"] = sol::nil;
        lua["on_collision"] = sol::nil;
   
        std::string script_path = components["script"]["path"];
        lua.script_file(script_path);

        // On click
        sol::optional<sol::function> has_on_click = lua["on_click"];
        sol::function on_click = sol::nil;

        if (has_on_click != sol::nullopt) {
            on_click = lua["on_click"];
        }

        // Update
        sol::optional<sol::function> has_update = lua["update"];
        sol::function update = sol::nil;

        if (has_update != sol::nullopt) {
            update = lua["update"];
        }

        // Collision
        sol::optional<sol::function> has_on_collision = lua["on_collision"];
        sol::function on_collision = sol::nil;

        if (has_on_collision != sol::nullopt) {
            on_collision = lua["on_collision"];
        }

        entity.addComponent<ScriptComponent>(update, on_click, on_collision);
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
            components["sprite"]["z_index"].get_or(0), // 0 default
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

void SceneLoader::loadText(Entity& entity, const sol::table& components) {
    sol::optional<sol::table> has_text = components["text"];

    if (has_text != sol::nullopt) {
        entity.addComponent<TextComponent>(
            components["text"]["text"],
            components["text"]["font_id"],
            components["text"]["r"],
            components["text"]["g"],
            components["text"]["b"],
            components["text"]["a"]
        );
    }
}

void SceneLoader::loadFonts(const sol::table& fonts, \
    std::unique_ptr<AssetManager>& asset_manager) {
    int index = 0;
    while (true) {
        sol::optional<sol::table> has_font = fonts[index];

        if (has_font == sol::nullopt) { // nothing
            break;
        }

        sol::table font = fonts[index];
        std::string font_id = font["font_id"];
        std::string file_path = font["file_path"];
        int size = font["font_size"];

        asset_manager->addFont(font_id, file_path, size);
        index++;
    }
}

void SceneLoader::loadButtons(const sol::table& buttons, \
    std::unique_ptr<ControllerManager>& controller_manager) {
    int index = 0;
    while (true) {
        sol::optional<sol::table> has_button = buttons[index];

        if (has_button == sol::nullopt) { // nothing
            break;
        }

        sol::table button = buttons[index];
        std::string button_name = button["name"];
        int button_code = button["button"];


        controller_manager->addMouseButton(button_name, button_code);
        index++;
    }
}

void SceneLoader::loadCameraFollow(Entity& entity, const sol::table& components) {
    sol::optional<sol::table> has_camera_follow = components["camera_follow"];

    if (has_camera_follow != sol::nullopt) {
        entity.addComponent<CameraFollowComponent>();
    }
}

//* Map
void SceneLoader::loadMap(SDL_Renderer* renderer, const sol::table map, std::unique_ptr<Registry>& registry, \
    std::unique_ptr<AssetManager>& asset_manager) {
    sol::optional<int> has_width = map["width"];

    if (has_width != sol::nullopt) {
        Game::getInstance().map_width = map["width"];
    }

    sol::optional<int> has_height = map["height"];
    if (has_height != sol::nullopt) {
        Game::getInstance().map_height = map["height"];
    }

    sol::optional<std::string> has_path = map["map_path"];
    if (has_path != sol::nullopt) {
        std::string map_path = map["map_path"];

        // An xml that contains the map data is loaded
        tinyxml2::XMLDocument xml_map;
        xml_map.LoadFile(map_path.c_str());

        // Extract the xml document's map root
        tinyxml2::XMLElement* xml_root = xml_map.RootElement();
        
        // Extract the map and tile's width and height
        int tile_width, tile_height, map_width, map_height;
        xml_root->QueryIntAttribute("tilewidth", &tile_width);
        xml_root->QueryIntAttribute("tileheight", &tile_height);

        xml_root->QueryIntAttribute("width", &map_width);
        xml_root->QueryIntAttribute("height", &map_height);

        // Calculate the map's width and height
        Game::getInstance().map_width = tile_width * map_width;
        Game::getInstance().map_height = tile_height * map_height;

        // Load document with tile data
        std::string tile_path = map["tile_path"];
        std::string tile_name = map["tile_name"];
        
        tinyxml2::XMLDocument xml_tileset;
        xml_tileset.LoadFile(tile_path.c_str());

        tinyxml2::XMLElement* xml_tileset_root = xml_tileset.RootElement();

        // Extract column count
        int columns;
        xml_tileset_root->QueryIntAttribute("columns", &columns);

        // Get the first element of 'layer' type
        tinyxml2::XMLElement* xml_layer = xml_root->FirstChildElement("layer");
        int layer_index = 0;
        while (xml_layer != nullptr) {
            loadLayer(renderer, registry, xml_layer,
            tile_width, tile_height, map_width, map_height,
            tile_name, columns, layer_index,
            asset_manager);
            std::cout << "[LAYER NAME]: " << xml_layer->Attribute("name") << " [LAYER INDEX]: " << layer_index << ".\n";
            xml_layer = xml_layer->NextSiblingElement("layer");
            

            layer_index++;
        }
    }
}

SDL_RendererFlip SceneLoader::getFlip(bool flip_d, bool flip_h, bool flip_v) {
    if (!flip_d && !flip_h && !flip_v) return SDL_FLIP_NONE;
    if (!flip_d &&  flip_h && !flip_v) return SDL_FLIP_HORIZONTAL;
    if (!flip_d && !flip_h &&  flip_v) return SDL_FLIP_VERTICAL;
    if (!flip_d &&  flip_h &&  flip_v) return SDL_FLIP_NONE;
    if ( flip_d && !flip_h && !flip_v) return SDL_FLIP_VERTICAL;
    if ( flip_d &&  flip_h && !flip_v) return SDL_FLIP_NONE;
    if ( flip_d && !flip_h &&  flip_v) return SDL_FLIP_NONE;
    if ( flip_d &&  flip_h &&  flip_v) return SDL_FLIP_VERTICAL;
    return SDL_FLIP_NONE;
}

double SceneLoader::getAngle(bool flip_d, bool flip_h, bool flip_v) {
    if (!flip_d && !flip_h && !flip_v) return 0;
    if (!flip_d &&  flip_h && !flip_v) return 0;
    if (!flip_d && !flip_h &&  flip_v) return 0;
    if (!flip_d &&  flip_h &&  flip_v) return 180;
    if ( flip_d && !flip_h && !flip_v) return 90;
    if ( flip_d &&  flip_h && !flip_v) return 90;
    if ( flip_d && !flip_h &&  flip_v) return 270;
    if ( flip_d &&  flip_h &&  flip_v) return 270;
    return 0;
}

void SceneLoader::renderTile(SDL_Renderer* renderer, SDL_Texture* tileset_texture, \
    uint32_t tile_id, \
    bool flip_h, bool flip_v, bool flip_d, \
    int tile_number, \
    int tile_width, int tile_height, \
    int map_width, int columns) {

    double angle = getAngle(flip_d, flip_h, flip_v);
    SDL_RendererFlip flip = getFlip(flip_d, flip_h, flip_v);

    SDL_Rect src = {
        ((static_cast<int>(tile_id - 1)) % columns) * tile_width,
        ((static_cast<int>(tile_id - 1)) / columns) * tile_height,
        tile_width,
        tile_height
    };

    SDL_Rect dst = {
        (tile_number % map_width) * tile_width,
        (tile_number / map_width) * tile_height,
        tile_width,
        tile_height
    };

    SDL_RenderCopyEx(renderer, tileset_texture, &src, &dst, angle, nullptr, flip);
}

void SceneLoader::processTile(SDL_Renderer* renderer, SDL_Texture* tileset_texture, \
    const std::string& number_str, \
    int tile_number, int tile_width, int tile_height, \
    int map_width, int columns) {

    if (number_str.empty()) {
        return;
    }

    const uint32_t FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
    const uint32_t FLIPPED_VERTICALLY_FLAG   = 0x40000000;
    const uint32_t FLIPPED_DIAGONALLY_FLAG   = 0x20000000;
    const uint32_t FLIP_MASK = ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);

    uint32_t raw_id  = static_cast<uint32_t>(std::stoull(number_str) & 0xFFFFFFFF);
    uint32_t tile_id = raw_id & FLIP_MASK;

    if (tile_id == 0) {
        return;
    };

    bool flip_h = (raw_id & FLIPPED_HORIZONTALLY_FLAG) != 0;
    bool flip_v = (raw_id & FLIPPED_VERTICALLY_FLAG)   != 0;
    bool flip_d = (raw_id & FLIPPED_DIAGONALLY_FLAG)   != 0;

    renderTile(renderer, tileset_texture, tile_id, \
        flip_h, flip_v, flip_d, \
        tile_number, \
        tile_width, tile_height, \
        map_width, columns);
}

SDL_Texture* SceneLoader::bakeLayer(SDL_Renderer* renderer, const char* data, \
    SDL_Texture* tileset_texture, \
    int tile_width, int tile_height, \
    int map_width, int map_height, int columns) {

    SDL_Texture* baked_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        map_width * tile_width,
        map_height * tile_height
    );

    if (!baked_texture) {
        std::cout << "[MAP] Failed to create texture: " << SDL_GetError() << std::endl;
        return nullptr;
    }
    
    if (SDL_SetRenderTarget(renderer, baked_texture) != 0) {
        std::cout << "[MAP] Failed to set render target: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    SDL_SetTextureBlendMode(baked_texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(tileset_texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    std::stringstream temp_number;
    int pos = 0;
    int tile_number = 0;

    while (true) {
        if (data[pos] == '\0') {
            if (!temp_number.str().empty()) {
                processTile(renderer, tileset_texture, temp_number.str(),
                    tile_number, tile_width, tile_height, map_width, columns);
            }
            break;
        }
        if (isdigit(data[pos])) {
            temp_number << data[pos];
        } else if (!temp_number.str().empty()) {
            processTile(renderer, tileset_texture, temp_number.str(),
                tile_number, tile_width, tile_height, map_width, columns);
            tile_number++;
            temp_number.str("");
            temp_number.clear();
        }
        pos++;
    }

    SDL_SetRenderTarget(renderer, nullptr);
    return baked_texture;
}

void SceneLoader::loadLayer(
    SDL_Renderer* renderer, std::unique_ptr<Registry>& registry, \
    tinyxml2::XMLElement* layer, \
    int tile_width, int tile_height, \
    int map_width, int map_height, \
    const std::string& tileset, int columns, int z_index, \
    std::unique_ptr<AssetManager>& asset_manager) {

    const char* data = layer->FirstChildElement("data")->GetText();

    SDL_Texture* baked_texture = bakeLayer(
        renderer, data,
        asset_manager->getTexture(tileset),
        tile_width, tile_height,
        map_width, map_height,
        columns
    );

    if (!baked_texture) {
        return;
    }

    Entity layer_entity = registry->createEntity();
    layer_entity.addComponent<TileMapComponent>(
        baked_texture,
        map_width * tile_width,
        map_height * tile_height
    );
    layer_entity.addComponent<LayerComponent>(z_index);
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
        
        Entity new_entity = registry->createEntity();

        sol::optional<sol::table> has_components = entity["components"];

        if (has_components != sol::nullopt) {
            sol::table components = entity["components"];

            loadText(new_entity, components);
            loadTag(new_entity, components);
            loadClickable(new_entity, components);
            loadAnimation(new_entity, components);
            loadCursor(new_entity, components);
            loadBoxCollider(new_entity, components);
            loadCircleCollider(new_entity, components);
            loadRigidbody(new_entity, components);
            loadSprite(new_entity, components);
            loadCameraFollow(new_entity, components);
            loadTransform(new_entity, components);
            loadScript(lua, new_entity, components);
            
        }

        index++;
    }
}

