#include <algorithm>
#include <iostream>

#include <stdlib.h>
#include <SDL3/SDL.h>

#include "NoteManager.h"
#include "GameConfig.h"
#include "Judgment.h"

// -------- 主构造函数 --------
NoteManager::NoteManager(const GameConfig& cfg, 
                        const std::vector<TrackConfig>& tracks,
                        const JudgmentWindows& windows): 
    config(cfg), tracks(tracks), windows(windows), gen_count_per_tick(1) {
    // 默认初始值为0，不生成音符
    next_beat_time = 0.0f;
    }

// -------------------------
// -------- 公共接口 --------
// -------------------------
void NoteManager::SetBPM(float BPM) {
    bpm = BPM;
    if (bpm > 0.0f) {
        RecalcBeatInterval();
        // 重置生成时钟，让生成从当前游戏时间开始
        // 这里不直接设置 m_nextBeatTime，将在 Update 里自动对齐
        std::cout << "BPM 被设置为: " << bpm << ", 基准note间隔时间: " 
        << beat_interval << " ms" << std::endl;
    }
}

void NoteManager::SetBeatFraction(float fraction) {
    beat_fraction = fraction;
}


void NoteManager::EnableRandomGeneration(bool enable) {
    random_gen_enabled = enable;
}

void NoteManager::SetGenerationProbabity(float prob) {
    if (prob < 0.0f) {
        prob = 0.0f;
    }
    if (prob > 1.0f) {
        prob = 1.0f;
    }
    gen_probability = prob;
}
void NoteManager::SetGenCountPerTick(int count) {
    if (count < 1) {
        count = 1;
    }
    gen_count_per_tick = count;
}

bool NoteManager::LoadChart(const std::string& filepath) {
    // TODO：后续要解析谱面文件，勿忘！
    return false;
}

void NoteManager::ClearChart() {
    notes.clear();
    next_beat_time = 0.0f;
}

// -------------------------
// -------- 内部辅助 --------
// -------------------------
void NoteManager::RecalcBeatInterval() {
    if (bpm > 0.0f) {
        beat_interval = 60000.0f / bpm;
    }
    else {
        beat_interval = 0.0f;
    }
}

void NoteManager::SortNotes() {
    std::sort(notes.begin(), notes.end(), 
            [](const Note& a, const Note& b) {
        return a.target_time < b.target_time;
    });
}

// -------- 每帧更新 -------- 
void NoteManager::Update(float musicTime) {
    // 漏键检测（标记飞出屏幕底部的音符为 missed）
    GenerateRandomNotes(musicTime);
    CheckMissedNotes(musicTime);
}

// -------- 生成逻辑 --------
void NoteManager::GenerateRandomNotes(float musicTime) {
    if (!random_gen_enabled || bpm <= 0.0f) {
        return;
    }
    // 对齐生成时钟：如果 m_nextBeatTime 落后于当前时间，则推进至一致。
    if (next_beat_time < musicTime) {
        next_beat_time = musicTime;
    }

    int loopGuard = 0;
    const int maxLoops = 1000;

    while (musicTime >= next_beat_time) {
        // 以概率决定在该拍点生成音符
        if ((rand() % 1000) / 1000.0f < gen_probability) {
            std::vector<int> track_indices(tracks.size());
            for (size_t i = 0; i < tracks.size(); ++i) {
                track_indices[i] = static_cast<int>(i);
            }

            for (size_t i = track_indices.size() - 1; i > 0; --i) {
                size_t j = rand() % (i + 1);
                std::swap(track_indices[i], track_indices[j]);
            }

            int actual_count = gen_count_per_tick;
            if (actual_count > static_cast<int>(tracks.size())) {
                actual_count = static_cast<int>(tracks.size());
            }

            if (actual_count == 1 && tracks.size() > 1) {
                // 避免连续生成同一轨道
                if (track_indices[0] == last_track) {
                    std::swap(track_indices[0], track_indices[1]);
                }
                last_track = track_indices[0];
            }

            for (int i = 0; i < actual_count; ++i) {
                int track = track_indices[i];
                // 固定起始的音符 Y 坐标
                float start_Y = - config.note_height - 10.0f;
                // 逆向计算音符到达判定线的时间
                float targetTime = musicTime + (config.judge_line_Y - start_Y) / config.speed;
                notes.push_back({track, targetTime, false, false});
            }
        }
        // 计算下一次生成时间：使用动态间隔
        next_beat_time += beat_interval * beat_fraction;
    }
    SortNotes();
}

// -------- 漏键检测 --------
void NoteManager::CheckMissedNotes(float musicTime) {
    for (auto& note : notes) {
        if (!note.hit && !note.missed) {
            float timeDiff = note.target_time - musicTime;
            float note_Y = config.judge_line_Y - timeDiff * config.speed;
            if (note_Y > config.screen_bottom) {
                note.missed = true;
                std::cout << "MISS (漏键) 轨道: " << tracks[note.track].key_name
                          << "\t时间: " << note.target_time << " ms" << std::endl;
            }
        }
    }
}

// -------- 按键判定 --------
bool NoteManager::HandleKeyPress(int keyCode, float musicTime) {
    for (auto& note : notes) {
        if (note.hit || note.missed) {
            continue;
        }
        // 找到对应轨道
        const auto& track = tracks[note.track];
        if (keyCode != track.keyCode) {
            continue;
        }

        float deviation = musicTime - note.target_time;
        float absDev = std::abs(deviation);
        if (absDev > windows.max_valid_window) {
            break;      // 因为音符按照时间排序，后续偏差会逐渐增大，直接跳出
        }

        // 直接调用判定函数判定按键结果
        JudgmentResult result = judgeNote(musicTime, note.target_time, windows);

        // 判定结果
        std::string gradeStr;
        switch (result.grade)
        {
            case JudgmentGrade::CRITICAL:
                gradeStr = "CRITICAL";
                break;
            case JudgmentGrade::PERFECT:
                gradeStr = "PERFECT";
                break;
            case JudgmentGrade::GREAT:
                gradeStr = "GREAT";
                break;
            case JudgmentGrade::GOOD:
                gradeStr = "GOOD";
                break;
            case JudgmentGrade::BAD:
                gradeStr = "BAD";
                break;
            case JudgmentGrade::MISS:
                gradeStr = "MISS";
                break;
        }
        std::string timingStr;
        switch (result.timing)
        {
            case JudgmentTiming::EARLY:
                timingStr = "EARLY";
                break;
            case JudgmentTiming::LATE:
                timingStr = "LATE";
                break;
            default:
                timingStr = "";
                break;
        }
        std::cout << tracks[note.track].key_name << "  " << gradeStr << "\t\t" << timingStr << "\t"
        << "  偏移:" << result.deviation << " ms" << std::endl;

        // 更新音符状态
        if (result.grade != JudgmentGrade::MISS) {
            note.hit = true;
        }
        else {
            note.missed = true;
        }
        return true; // 处理了一个音符，成功
    }
    return false; // 没有找到可处理的音符
}

// -------- 渲染 --------
void NoteManager::Render(SDL_Renderer* renderer, float music_time) {
    for (const auto& note : notes) {
        if (note.hit || note.missed) {
            continue;
        }

        float timeDiff = note.target_time - music_time;
        float note_Y = config.judge_line_Y - timeDiff * config.speed;
        
        if (note_Y > -20.0f && note_Y < config.screen_bottom) {
            const auto& track = tracks[note.track];
            SDL_FRect rect = {track.center_X - track.width / 2.0f, note_Y,
                                track.width, config.note_height};
            SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}