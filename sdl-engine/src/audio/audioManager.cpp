#include "audioManager.hpp"
#include <iostream>
#include <algorithm>
#include <random>
#include <vector>

AudioManager::AudioManager() {
}

AudioManager::~AudioManager() {
    clearAssets();
}

Mix_Chunk* AudioManager::loadSound(const std::string& file_path) {
    auto it = sound_effects.find(file_path);
    if (it != sound_effects.end()) {
        return it->second;
    }

    Mix_Chunk* chunk = Mix_LoadWAV(file_path.c_str());
    if (!chunk) {
        std::cerr << "[AUDIO] Failed to load sound '" << file_path << "': "
                  << Mix_GetError() << std::endl;
        return nullptr;
    }

    sound_effects[file_path] = chunk;
    return chunk;
}

Mix_Music* AudioManager::loadMusic(const std::string& file_path) {
    auto it = music_tracks.find(file_path);
    if (it != music_tracks.end()) {
        return it->second;
    }

    Mix_Music* music = Mix_LoadMUS(file_path.c_str());
    if (!music) {
        std::cerr << "[AUDIO] Failed to load music '" << file_path << "': "
                  << Mix_GetError() << std::endl;
        return nullptr;
    }

    music_tracks[file_path] = music;
    return music;
}

bool AudioManager::playSound(const std::string& file_path, int loops, int volume) {
    Mix_Chunk* chunk = loadSound(file_path);
    if (!chunk) {
        return false;
    }

    // Clamp volume to valid range (0-128)
    volume = std::max(0, std::min(128, volume));
    
    // Set volume for this chunk (affects all instances of this sound)
    Mix_VolumeChunk(chunk, volume);

    if (Mix_PlayChannel(-1, chunk, loops) == -1) {
        std::cerr << "[AUDIO] Failed to play sound '" << file_path << "': "
                  << Mix_GetError() << std::endl;
        return false;
    }

    return true;
}

bool AudioManager::playMusic(const std::string& file_path, int loops, int volume) {
    Mix_Music* music = loadMusic(file_path);
    if (!music) {
        return false;
    }

    // Clamp volume to valid range (0-128)
    volume = std::max(0, std::min(128, volume));
    
    // Set music volume
    Mix_VolumeMusic(volume);

    if (Mix_PlayMusic(music, loops) == -1) {
        std::cerr << "[AUDIO] Failed to play music '" << file_path << "': "
                  << Mix_GetError() << std::endl;
        return false;
    }

    return true;
}

bool AudioManager::playRandomSound(const std::vector<std::string>& file_paths, int loops, int volume) {
    if (file_paths.empty()) {
        return false;
    }

    // Select a random path
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, file_paths.size() - 1);
    std::string selected_path = file_paths[dis(gen)];

    return playSound(selected_path, loops, volume);
}

void AudioManager::stopMusic() {
    Mix_HaltMusic();
}

void AudioManager::clearAssets() {
    stopMusic();

    for (auto& pair : sound_effects) {
        if (pair.second) {
            Mix_FreeChunk(pair.second);
        }
    }
    sound_effects.clear();

    for (auto& pair : music_tracks) {
        if (pair.second) {
            Mix_FreeMusic(pair.second);
        }
    }
    music_tracks.clear();
}
