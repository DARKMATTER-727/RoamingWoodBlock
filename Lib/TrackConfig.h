#pragma once

#include <string>
#include <vector>

#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL.h>

#include "GameConfig.h"

struct TrackConfig {
    int id;
    SDL_Keycode keyCode;
    std::string key_name;       //调试输出用
    float center_X = 0.0f;      //音符绘制基准点
    float width;                //音符宽度
};

inline std::vector<TrackConfig> CreateDefaultTracks(const GameConfig& config) {
    std::vector<TrackConfig> tracks;
    tracks.reserve(config.track_count);
    // 默认键位（可自定义）
    SDL_Keycode default_keys[] = {SDLK_Z, SDLK_X, SDLK_COMMA, SDLK_PERIOD};
    std::string default_names[] = {"Z", "X", ",", "."};
    for (int i = 0; i < config.track_count; ++i) 
    {
        float cx = config.track_width * (i + 0.5f);   // 轨道中心 = (i+0.5) * 轨道宽
        tracks.push_back(
            {
                i, default_keys[i], 
                default_names[i],
                cx, config.note_width
            }
        );
    }
    return tracks;
}