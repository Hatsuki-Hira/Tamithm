#include <stdio.h>
#include <conio.h>

#include "../global.h"
#include "../data/renderer.h"
#include "../data/ascii_letters.h"
#include "../data/audio_system.h"



float acc = 0;
int score = 0;
int level = 0;

void score_caculate(void) {
    acc = (score_perfect * JUDGE_PERFECT_HEAVY + score_good * JUDGE_GOOD_HEAVY + score_bad * JUDGE_BAD_HEAVY) / all_note_count;
    //score = (int)(acc * 900000.0f) + 100000 * max_combo / all_note_count;
    score = (int)(acc * 1000000.0f);
    if (score > 1000000) score = 1000000;
}

void level_caculate(void) {
    if (score < 700000) level = 0;
    else if (700000 <= score && score < 820000) level = 1;
    else if (820000 <= score && score < 880000) level = 2;
    else if (880000 <= score && score < 920000) level = 3;
    else if (920000 <= score && score < 960000) level = 4;
    else if (960000 <= score && score < 980000) level = 5;
    else level = 6;
}

void update_result_ui(void) {
    screen_clear(&screen0);
    SLEEP_MS(400);

    int y = user_config.height / 3;
    int x = user_config.width / 3;

    // 计算分数并分级
    score_caculate();
    level_caculate();

    /* ---------- 绘制等级 ---------- */
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 5; col++) {
            screen_set_cell(&screen0, y + row, x + col, level_font[level][row][col], 15, COLOR_NONE);
        }
    }
    // 等级宽5列 + 1列间距
    x += 16;


    /* ---------- 按位数拆数字 ---------- */
    int digits[7];
    int dcount = 0;
    int temp_score = score;

    if (temp_score == 0) {
        digits[dcount++] = 0;
    } else {
        while (temp_score > 0) {
            digits[dcount++] = temp_score % 10;
            temp_score /= 10;
        }
    }

    /* ---------- 绘制分数详情 ---------- */
    int fast_count = score_good_fast + score_bad_fast;
    int late_count = score_good_late + score_bad_late;

    char line[12][128];
    sprintf(line[0],  "   Accuracy   %.2f%%", acc * 100.0f);
    sprintf(line[1],  " ");
    sprintf(line[2],  "  MAX COMBO   %d", max_combo);
    sprintf(line[3],  " ");
    sprintf(line[4],  "    PERFECT   %d", score_perfect);
    sprintf(line[5],  "       GOOD   %d", score_good);
    sprintf(line[6],  "        BAD   %d", score_bad);
    sprintf(line[7],  "       MISS   %d", score_miss);
    sprintf(line[8],  " ");
    sprintf(line[9],  "    Fast %d   Late %d", fast_count, late_count);
    sprintf(line[10], " ");
    sprintf(line[11], "<press Enter to continue>");
    for (int i = 0; i < 12; i++) {
        screen_display_text(&screen0, y + 5 + i, x, line[i], 7, COLOR_NONE);
    }

    /* ---------- 绘制数字 ---------- */
    for (int d = dcount - 1; d >= 0; d--) {
        int digit = digits[d];
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                screen_set_cell(&screen0, y + row, x + col, combo_font[digit][row][col], 15, COLOR_NONE);
            }
        }
        // 数字宽3列 + 1列间距
        x += 4;
    }

    // 转场
    if (play_transition_animation) {transition_animation_fade_out(); play_transition_animation = 0;}

    render(&screen0, 1);
}



void handle_result_input(void) {
    if (_kbhit())
    {
        int key = _getch();
        // 按 Enter 继续
        if (key == 13) {
            audio_exit();

            // 转场
            play_transition_animation = 1;
            transition_animation_fade_in(); SLEEP_MS(600);

            game_state = STATE_SONG_SELECT;
        }
    }
}