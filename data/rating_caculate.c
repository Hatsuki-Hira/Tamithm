/* 计算流程
1.
individual strain decay

2.
individual += IndividualEvaluator

3.
highestIndividualStrain

4.
overall decay

5.
overall += OverallEvaluator

6.
StrainValueOf()

返回：

highestIndividualStrain
+
overallStrain
-
CurrentStrain
*/
#include "rating_caculate.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LANES 18
#define MAX_SECTIONS 50000

#define INDIVIDUAL_DECAY 0.125
#define OVERALL_DECAY 0.30
#define SECTION_LENGTH 400.0
#define STAR_MULTIPLIER 0.018

// 辅助函数：获取 Note 的真实结束时间（兼容 Tamithm 中 Tap 设为 -1 的情况）
static inline double get_end_time(Note *n) {
    return (n->type == NOTE_HOLD && n->end_time > 0) ? (double)n->end_time : (double)n->start_time;
}

static double decay(double value, double delta, double base) {
    return value * pow(base, delta / 1000.0);
}

static double logistic(double x, double midpoint, double multiplier) {
    return 1.0 / (1.0 + exp(multiplier * (midpoint - x)));
}

// 辅助排序函数：强制按 startTime 升序排列
static int compare_notes_by_time(const void *a, const void *b) {
    Note *na = (Note *)a;
    Note *nb = (Note *)b;
    if (na->start_time < nb->start_time) return -1;
    if (na->start_time > nb->start_time) return 1;
    return 0;
}

// 用于段落 Peak 降序排列
static int compare_desc(const void *a, const void *b) {
    double da = *(double*)a;
    double db = *(double*)b;
    return (da < db) ? 1 : ((da > db) ? -1 : 0);
}

double mania_calculate_rating(Note *notes, int note_count, int lane_count) {
    if (note_count <= 1) return 0.0;

    // 1. 确保 Note 按时间排序（等价于 Osu 源码中的 LegacySortHelper）
    qsort(notes, note_count, sizeof(Note), compare_notes_by_time);

    double individual_strains[MAX_LANES] = {0};
    double highest_individual_strain = 0.0;
    double overall_strain = 1.0; // Osu 源码在 Strain.cs 构造函数中初始化为 1

    Note *previous_lane[MAX_LANES] = {NULL};
    // Osu 是从第 2 个 Note（索引1）开始实际计算的，第 1 个只作为时间锚点
    previous_lane[notes[0].lane] = &notes[0];
    Note *previous_global = &notes[0];

    double peaks[MAX_SECTIONS] = {0};
    int peak_count = 0;

    // 初始 Section 边界
    double section_end = ceil((double)notes[1].start_time / SECTION_LENGTH) * SECTION_LENGTH;
    double current_peak = 0.0;

    // 从第 2 个 Note 开始循环
    for (int i = 1; i < note_count; i++) {
        Note *curr = &notes[i];
        double curr_start = (double)curr->start_time;
        double curr_end = get_end_time(curr);

        // --- 边界处理：跨越 Section 时的衰减计算 (对应 Osu 的 startNewSectionFrom) ---
        while (curr_start > section_end) {
            peaks[peak_count++] = current_peak;
            // 核心修正：跨段不应该归零，而是计算到跨段时刻的衰减值作为新段的初始峰值
            double offset = section_end - previous_global->start_time;
            current_peak = decay(highest_individual_strain, offset, INDIVIDUAL_DECAY) +
                           decay(overall_strain, offset, OVERALL_DECAY);
            section_end += SECTION_LENGTH;
        }

        double delta_time = curr_start - previous_global->start_time;
        // 如果该轨道是首个 Note，默认经过的时间为它的绝对 start_time
        double column_time = previous_lane[curr->lane] ? 
                             curr_start - previous_lane[curr->lane]->start_time : curr_start;

        // --- 1. Evaluate Individual ---
        double hold_factor_ind = 1.0;
        for (int k = 0; k < lane_count; k++) {
            Note *prev = previous_lane[k];
            if (!prev) continue;
            double prev_start = (double)prev->start_time;
            double prev_end = get_end_time(prev);

            // 对应 Precision.DefinitelyBigger (允许1ms容差)
            if (prev_end - curr_end > 1.0 && curr_start - prev_start > 1.0) {
                hold_factor_ind = 1.25;
                break;
            }
        }
        double individual_eval = 2.0 * hold_factor_ind;

        individual_strains[curr->lane] = decay(individual_strains[curr->lane], column_time, INDIVIDUAL_DECAY);
        individual_strains[curr->lane] += individual_eval;

        // --- 2. Update Highest Individual Strain (多押防重算优化) ---
        if (delta_time <= 1.0) { // 同一时间落下的 Chord
            highest_individual_strain = fmax(highest_individual_strain, individual_strains[curr->lane]);
        } else {
            highest_individual_strain = individual_strains[curr->lane];
        }

        // --- 3. Evaluate Overall ---
        double closest_end = fabs(curr_end - curr_start);
        int is_overlapping = 0;
        double hold_factor_ovr = 1.0;
        double hold_addition = 0.0;

        for (int k = 0; k < lane_count; k++) {
            Note *prev = previous_lane[k];
            if (!prev) continue;
            double prev_start = (double)prev->start_time;
            double prev_end = get_end_time(prev);

            is_overlapping |= (prev_end - curr_start > 1.0) &&
                              (curr_end - prev_end > 1.0) &&
                              (curr_start - prev_start > 1.0);

            if (prev_end - curr_end > 1.0 && curr_start - prev_start > 1.0) {
                hold_factor_ovr = 1.25;
            }

            closest_end = fmin(closest_end, fabs(curr_end - prev_end));
        }

        if (is_overlapping) {
            hold_addition = logistic(closest_end, 30.0, 0.27);
        }
        double overall_eval = (1.0 + hold_addition) * hold_factor_ovr;

        overall_strain = decay(overall_strain, delta_time, OVERALL_DECAY);
        overall_strain += overall_eval;

        // --- 4. Update Current Strain ---
        double current_strain = highest_individual_strain + overall_strain;
        if (current_strain > current_peak) {
            current_peak = current_strain;
        }

        // 更新历史引用
        previous_lane[curr->lane] = curr;
        previous_global = curr;
    }

    // 保存最后一个 Section 的 Peak
    peaks[peak_count++] = current_peak;

    // --- 加权求和 (Weighted Sum) ---
    qsort(peaks, peak_count, sizeof(double), compare_desc);
    double difficulty = 0.0;
    double weight = 1.0;

    for (int i = 0; i < peak_count; i++) {
        // osu!源码中，Peak为0的段不参与计算以优化最坏情况性能
        if (peaks[i] > 0) {
            difficulty += peaks[i] * weight;
            weight *= 0.9;
        }
    }

    return difficulty * STAR_MULTIPLIER;
}