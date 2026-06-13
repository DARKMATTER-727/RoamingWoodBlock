#include <iostream>

#include <windows.h>
#include <stdlib.h>
#include <time.h>

#include "Game.h"

Game::Game(): 
    config(), 
    windows(), 
    tracks(), 
    note_manager(std::make_unique<NoteManager>(config, tracks, windows)) {
    
    srand(static_cast<unsigned int>(time(nullptr)));
    // UTF-8
    SetConsoleOutputCP(65001);

    // 加载配置（目前可暂时在此修改）
    config.window_width     = 720;
    config.window_height    = 960;
    config.track_count      = 4;
    config.judge_line_ratio = 0.8f;
    config.note_width_ratio = 0.6f;
    config.note_height      = 10.0f;
    config.speed            = 0.950f;

    config.Recalc();

    windows.critical_window = 35.0f;
    windows.perfect_window  = 65.0f;
    windows.great_window    = 100.0f;
    windows.good_window     = 125.0f;
    windows.max_valid_window  = 250.0f;

    tracks = CreateDefaultTracks(config);

    note_manager = std::make_unique<NoteManager>(config, tracks, windows);
    
    note_manager -> SetGenCountPerTick(1);
    note_manager -> EnableRandomGeneration(true);
    note_manager -> SetBPM(200.000f);
    note_manager -> SetGenerationProbabity(1.00f);
    note_manager -> SetBeatFraction(0.5f);
    

    // 检测 SDL3 初始化情况
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL 初始化失败：" 
        << SDL_GetError() << std::endl;
        exit(-1);
    }
    // 生成游戏窗口，并检测窗口创建情况
    window = SDL_CreateWindow("滚木块", 
                config.window_width, 
                config.window_height, 
                0);
    if (!window) {
        std::cerr << "窗口创建失败：" 
        << SDL_GetError() << std::endl;
        exit(-1);
    }
    // 生成渲染器，并检测渲染器创建情况
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "渲染器创建失败：" 
        << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        exit(-1);
    }
    // 设置垂直同步（默认为关闭）
    SDL_SetRenderVSync(renderer, 0);

    start_timing = SDL_GetTicks();
}

Game::~Game() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::Run() {
    while (running) {
        float music_time = static_cast<float>(SDL_GetTicks() - start_timing);

        ProcessEvents();
        Update(music_time);
        Render();

        SDL_Delay(1);
    }
}

void Game::ProcessEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
        }
        if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
            note_manager -> HandleKeyPress(event.key.key, 
                static_cast<float>(SDL_GetTicks() - start_timing));
        }
    }    
}

void Game::Update(float music_time) {
    note_manager -> Update(music_time);
}

void Game::Render() {
    // 渲染背景
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    // 渲染判定线
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderLine(renderer, 10, config.judge_line_Y, 
        config.window_width - 10, config.judge_line_Y);

    // 渲染轨道分隔线
    for (int i = 1; i < config.track_count; ++i) {
        float x = config.track_width * i;
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
        SDL_RenderLine(renderer, x, 0, x, config.window_height);
    }

    // 渲染所有音符
    note_manager -> Render(renderer, 
        static_cast<float>(SDL_GetTicks() - start_timing));

    SDL_RenderPresent(renderer);
}