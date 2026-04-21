#ifndef TILE_RENDER_SYSTEM_HPP
#define TILE_RENDER_SYSTEM_HPP
#include "../e.c.s./ecs.hpp"
#include "../components/tileMapComponent.hpp"
#include "../components/layerComponent.hpp"

class TileRenderSystem : public System {
public:
    TileRenderSystem() {
        requireComponent<TileMapComponent>();
        requireComponent<LayerComponent>();
    }

    std::vector<Entity> getEntities() {
        return getSystemEntities();
    }
};
#endif