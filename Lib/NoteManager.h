#pragma once

#include <vector>
#include <string>

#include "GameConfig.h"
#include "Judgment.h"
#include "TrackConfig.h"

#include <SDL3/SDL.h>

struct Note {
    int track;
    float target_time;
    bool hit;
    bool missed;
};

class NoteManager {
private:
    const GameConfig& config;
    const std::vector<TrackConfig>& tracks;
    const JudgmentWindows& windows;

    std::vector<Note> notes;

    bool random_gen_enabled = true;         // 是否启动随机生成
    float gen_probability;                  // 随机生成概率

    float bpm;                              // 基准BPM（默认为0）
    float beat_interval;                    // 一拍的时间（毫秒，由BPM计算得到）
    float beat_fraction;                    // 标准四分拍点基础单位（默认一拍）
    int gen_count_per_tick;                 // 每次生成几个音符
    float next_beat_time = 0.0f;            // 下一个生成检查的时间点（毫秒）

    int last_track = -1;                    // 上一个生成的轨道（避免连续生成同一轨道）

    void GenerateRandomNotes(float music_time);
    void CheckMissedNotes(float music_time);
    void SortNotes();
    void RecalcBeatInterval();

public:
    NoteManager(const GameConfig& cfg, const std::vector<TrackConfig>& tracks, 
                const JudgmentWindows& windows);
    
    void Update(float music_time);                          // 每帧调用
    void Render(SDL_Renderer* renderer, float music_time);  // 渲染所有未处理的音符
    bool HandleKeyPress(int key_code, float music_time);    // 处理按键：返回 true 表示该按键被处理
 
    void SetBPM(float bpm);                     // 设置BPM，若值为0或负数时默认无效，停止生成    
    void SetBeatFraction(float fraction);       // 拍点位置分隔，我们默认 1.0 为四分之一拍，0.5 为八分之一拍，以此类推
    void EnableRandomGeneration(bool enable);   // 启用/禁用随机生成
    void SetGenerationProbabity(float prob);    // 在每个拍点生成音符的概率（0.0 ~ 1.0）（如果开启了随机生成）
    void SetGenCountPerTick(int count);         // 每个拍点生成的音符数量

    bool LoadChart(const std::string& filepath);    // 从文件读取BPM、谱面、流速等，结果返回成功/失败
    void ClearChart();                              // 清空当前谱面
};