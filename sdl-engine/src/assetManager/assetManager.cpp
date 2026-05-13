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
    if (!surface) {
        std::cerr << "[ASSET MANAGER] texture load failed: " << IMG_GetError() << " - " << file_path << std::endl;
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        std::cerr << "[ASSET MANAGER] texture creation failed: " << SDL_GetError() << " - " << file_path << std::endl;
        return;
    }

    textures.emplace(textureId, texture);
}

SDL_Texture* AssetManager::getTexture(const std::string& textureId) {
    auto it = textures.find(textureId);
    return it != textures.end() ? it->second : nullptr;
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
    auto it = fonts.find(font_id);
    return it != fonts.end() ? it->second : nullptr;
}