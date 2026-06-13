#pragma once

struct GameConfig {
    int window_width;
    int window_height;
    int track_count;            // 轨道数量
    float judge_line_ratio;     // 判定线高度占窗口高度的比例
    float note_width_ratio;     // 音符宽度占单轨道宽度的比例
    float note_height;          // 音符高度（固定像素）
    float speed;                // 像素/毫秒

    // 派生参数
    float judge_line_Y;
    float screen_bottom;
    float track_width;
    float note_width;

    GameConfig() {
        Recalc();
    }

    void Recalc() {
        judge_line_Y = window_height * judge_line_ratio;
        screen_bottom = window_height + note_height;
        track_width = static_cast<float>(window_width) / track_count;
        note_width = track_width * note_width_ratio;
    }
};