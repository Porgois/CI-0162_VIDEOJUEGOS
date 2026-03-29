#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <map>
#include <string>

class AssetManager {
    private:
        std::map<std::string, SDL_Texture*> textures;
        
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

};

#endif // ASSET_MANAGER_HPP