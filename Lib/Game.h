#pragma once

#include <memory>

#include "GameConfig.h"
#include "Judgment.h"
#include "NoteManager.h"
#include "TrackConfig.h"

class Game {
    private:
        void ProcessEvents();
        void Update(float music_time);
        void Render();

        GameConfig config;
        JudgmentWindows windows;
        std::vector<TrackConfig> tracks;
        std::unique_ptr<NoteManager> note_manager;

        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        bool running = true;
        Uint32 start_timing;
    public:
        Game();
        ~Game();
        void Run();
};