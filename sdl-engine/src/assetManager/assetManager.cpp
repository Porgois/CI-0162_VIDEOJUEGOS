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

    for (auto font : fonts) {
        TTF_CloseFont(font.second);
    }
    fonts.clear();
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

void AssetManager::addFont(const std::string& font_id, const std::string& file_path, int font_size) {
    TTF_Font* font = TTF_OpenFont(file_path.c_str(), font_size);

    if (font == NULL) {
        std::string error = TTF_GetError();
        std::cerr << "[ASSET MANAGER] font error: " << error << "!" << std::endl;
        return;
    }

    fonts.emplace(font_id, font);
}

TTF_Font* AssetManager::getFont(const std::string& font_id) {
    return fonts[font_id];
}