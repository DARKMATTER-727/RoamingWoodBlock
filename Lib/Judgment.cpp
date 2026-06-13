#include "Judgment.h"
#include <math.h>

JudgmentResult judgeNote(float hit_time, float target_time, const JudgmentWindows & windows)
{
    float deviation = hit_time - target_time;
    float absDev = fabsf(deviation);

    JudgmentResult result;
    result.deviation = deviation;

    // note 判定
    if (absDev <= windows.critical_window) {
        result.grade = JudgmentGrade::CRITICAL;
    }
    else if (absDev <= windows.perfect_window) {
        result.grade = JudgmentGrade::PERFECT;
    }
    else if (absDev <= windows.great_window) {
        result.grade = JudgmentGrade::GREAT;
    }
    else if (absDev <= windows.good_window) {
        result.grade = JudgmentGrade::GOOD;
    }
    else if (deviation < 0.0f) {
        result.grade = JudgmentGrade::BAD;
    }
    else {
        result.grade = JudgmentGrade::MISS;
    }
    
    // Early / Late 判定
    if (result.grade == JudgmentGrade::CRITICAL || result.grade == JudgmentGrade::BAD || result.grade == JudgmentGrade::MISS) {
        result.timing = JudgmentTiming::NONE;
    }
    else if (deviation < 0.0f) {
        result.timing = JudgmentTiming::EARLY;
    }
    else if (deviation > 0.0f) {
        result.timing = JudgmentTiming::LATE;
    }
    else {
        result.timing = JudgmentTiming::NONE;
    }
    return result;
}