#pragma once

#include <vector>
#include <string>

#include "GameConfig.h"

//判定等级
enum class JudgmentGrade {
    CRITICAL,
    PERFECT,
    GREAT,
    GOOD,
    BAD,
    MISS
};

//时机类型
enum class JudgmentTiming {
    EARLY,
    NONE,
    LATE
};

struct JudgmentResult {
    JudgmentGrade grade;
    JudgmentTiming timing;
    float deviation;        //具体早晚由该变量确定
};

struct JudgmentWindows {
    float critical_window;
    float perfect_window;
    float great_window;
    float good_window;
    float max_valid_window;
};

//判定函数
JudgmentResult judgeNote(float hittiming, float targettiming, 
    const JudgmentWindows & windows);