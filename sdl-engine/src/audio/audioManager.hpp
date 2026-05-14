#ifndef AUDIO_MANAGER_HPP
#define AUDIO_MANAGER_HPP

#include <SDL2/SDL_mixer.h>
#include <map>
#include <string>
#include <vector>

class AudioManager {
    public:
        AudioManager();
        ~AudioManager();

bool playSound(const std::string& file_path, int loops = 0, int volume = 128);
    bool playMusic(const std::string& file_path, int loops = -1, int volume = 128);
        bool playRandomSound(const std::vector<std::string>& file_paths, int loops = 0, int volume = 128);
        void stopMusic();
        void clearAssets();

    private:
        Mix_Chunk* loadSound(const std::string& file_path);
        Mix_Music* loadMusic(const std::string& file_path);

        std::map<std::string, Mix_Chunk*> sound_effects;
        std::map<std::string, Mix_Music*> music_tracks;
};

#endif // AUDIO_MANAGER_HPP
