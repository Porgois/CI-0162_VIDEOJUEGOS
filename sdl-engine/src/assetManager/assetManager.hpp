#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <map>
#include <string>

class AssetManager {
    private:
        std::map<std::string, SDL_Texture*> textures;
        std::map<std::string, TTF_Font*> fonts;
        
    public:
        AssetManager();
        ~AssetManager();

        void clearAssets();

        void addTexture(
            SDL_Renderer* renderer, 
            const std::string& textureId,
            const std::string& file_path
        );

        SDL_Texture* getTexture(const std::string& textureId);

        void addFont(const std::string& font_id, const std::string& file_path, int font_size);

        TTF_Font* getFont(const std::string& font_id);

};

#endif // ASSET_MANAGER_HPP