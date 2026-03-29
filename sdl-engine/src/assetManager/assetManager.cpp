#include "assetManager.hpp"

AssetManager::AssetManager() {
    std::cout << "[ASSET MANAGER] Executes constructor." << std::endl;
}

AssetManager::~AssetManager() {
    std::cout << "[ASSET MANAGER] Executes destructor." << std::endl;
}

void AssetManager::clearAssets() {
    for (auto texture : textures) {
        SDL_DestroyTexture(texture.second);
    }
    textures.clear();
}

void AssetManager::addTexture(SDL_Renderer* renderer, const std::string& textureId, \
    const std::string& file_path
) {
    SDL_Surface* surface = IMG_Load(file_path.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    textures.emplace(textureId, texture);
}

SDL_Texture* AssetManager::getTexture(const std::string& textureId) {
    return textures[textureId];
}