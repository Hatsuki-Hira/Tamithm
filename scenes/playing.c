#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <time.h>

#include "../global.h"
#include "../data/renderer.h"
#include "../data/ascii_letters.h"
#include "../data/audio_system.h"
#include "scenes.h"





// 窗口坐标（全局变量定义）
int playing_windows[2][2];
int lane_line[2][2];
int fast_late_position[2];

// 初始化窗口坐标
// 每个窗口用 [左上x, 左上y, 右下x, 右下y] 表示
void init_playing_windows(void) {
    playing_windows[0][0] = 3;  // 左上y
    playing_windows[0][1] = 0;  // 左上x
    playing_windows[1][0] = user_config.height - 3;            // 右下y
    playing_windows[1][1] = user_config.width - 1;  // 右下x

    int lane_line_middle = (int)(user_config.width * 0.5);  // 轨道中心
    lane_line[0][0] = 4;
    lane_line[0][1] = lane_line_middle - (int)((user_config.lane_padding + 2) * 2);  // 轨道左侧
    lane_line[1][0] = user_config.height - 3;
    lane_line[1][1] = lane_line[0][1] + (user_config.lane_padding + 1) * 4;  // 轨道右侧

    // 快慢指示器位置
    fast_late_position[0] = lane_line[0][0] + 0.6 *(lane_line[1][0] - lane_line[0][0]);  // y
    fast_late_position[1] = lane_line_middle - 4;  // x
}

// 显示 COMBO + 数字
static void draw_combo(void)
{
    if (combo == 0) return;
    int x = lane_line[1][1] + 1;
    int y = lane_line[0][0] + 4;

    /* ---------- 绘制 COMBO ---------- */
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 20; col++) {
            screen_set_cell(&screen0, y + row, x + col, combo_logo[row][col], 183, COLOR_NONE
            );
        }
    }

    // COMBO logo 宽20列 + 2列间距
    x += 23;

    /* ---------- 按位数拆数字 ---------- */
    int n = combo;
    int digits[10];
    int dcount = 0;

    if (n == 0) {
        digits[dcount++] = 0;
    } else {
        while (n > 0) {
            digits[dcount++] = n % 10;
            n /= 10;
        }
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
}




// 快慢指示器持续时间（毫秒）
#define FAST_LATE_DISPLAY_MS 400
// 快慢指示器状态
static int fast_late_type = 0;       // 0=fast, 1=late
static clock_t fast_late_time = 0;   // 上次触发时的 clock() 值

// 记录快慢判定（由 tap_calc_judge 调用）
static void record_fast_late(int type) {
    fast_late_type = type;
    fast_late_time = clock();
}

// 绘制快慢指示器（如果还在显示窗口内）
void draw_fast_late(void) {
    clock_t now = clock();
    int elapsed_ms = (int)((now - fast_late_time) * 1000 / CLOCKS_PER_SEC);
    if (elapsed_ms > FAST_LATE_DISPLAY_MS) return;  // 超时不显示

    switch(fast_late_type) {
        case 0: screen_display_text(&screen0, fast_late_position[0], fast_late_position[1], "fa", 81, COLOR_NONE);
                screen_display_text(&screen0, fast_late_position[0], fast_late_position[1] + 3, "st", 81, COLOR_NONE); break;
        case 1: screen_display_text(&screen0, fast_late_position[0], fast_late_position[1], "la", 202, COLOR_NONE);
                screen_display_text(&screen0, fast_late_position[0], fast_late_position[1] + 3, "te", 202, COLOR_NONE); break;
    }
}




void draw_score(void) {
    score_caculate();
    int y = 4;
    int x = user_config.width - 28;
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
}




// 获取当前游戏进行时间（毫秒）
static int get_game_time_ms(void) {
    int num = (clock() - game_start_time) * 1000 / CLOCKS_PER_SEC;
    if (user_config.music_offset < 0) num += user_config.music_offset;
    return num;
}



// 将 Note 的 start_time 映射为浮点屏幕 Y 坐标

/*  参数:
        note_time    - Note 的 start_time（毫秒）
        judge_line_y - 判定线在屏幕上的行数（整数）
    返回值:
        浮点数 Y，分量在 [窗口顶 ~ 判定线] 之间
        如果 note 还没进入可见区，返回窗口顶（clamp 在 top_y）
        如果 note 已经过判定线，返回判定线（clamp 在 judge_y）
    计算:
        1. progress = diff / VISIBLE_TIME_MS
           progress 从 1.0（刚进入视野）到 0.0（到达判定线）
        2. Y = judge_y - progress * (judge_y - top_y)
           progress=1 → Y = top_y       （窗口顶部）
           progress=0 → Y = judge_y      （判定线上）*/
static float note_time_to_y(int note_time, int judge_line_y) {
    int current_ms = get_game_time_ms();
    int diff = note_time - current_ms;  // >0: note 未来, <=0: note 已过

    // progress 范围 [0, 1]，1=刚可见，0=到判定线
    float progress = diff / (float)user_config.note_speed;  // 可见时间范围（毫秒）
    if (progress < -0.2f) progress = -0.2f;  // 设置<0的负值，可以在判定线下面继续下落一点点，也防止尾部渲染问题
    if (progress > 1.1f) progress = 1.1f;  // 设置>1的正值，令Note出现位置在框外，防止头部渲染问题

    int top_y = playing_windows[0][0] + 1;
    return (judge_line_y - progress * (judge_line_y - top_y));
}



// ──────────────────── Note 零件绘制 ────────────────────────
// 在指定格子画一个 Note
// half: 0=▄(下半个), 1=▀(上半个), 2=整格(白底空格)
static void draw_note_cell(int y, int x, int half, int color) {
    if (half == 2) {
        screen_set_cell(&screen0, y, x, " ", COLOR_NONE, color);
    } else {
        screen_set_cell(&screen0, y, x, half ? "▀" : "▄", color, COLOR_INHERIT);  // 半格不覆盖背景色
    }
}



// ──────────────────────── Note 完整绘制 ────────────────────────
/*  参数:
    lane    - 轨道号 (0 ~ keys-1)
    y       - Note 中心位置的浮点屏幕 Y 坐标
    color   - 字符颜色
    if_judge_line_can_cover - 过了判定线不渲染 (用于 Hold)*/
static void draw_note_shape(int lane, float y, int color, int if_judge_line_can_cover) {
    int keys = chart_info.keys;
    if (keys < 1) keys = 4;

    int x0 = playing_windows[0][1] + 1;
    int x1 = playing_windows[1][1] - 1;
    int inner_w = x1 - x0 + 1;

    // ──────────────────────── Note位置判断与绘制 ────────────────────────
    /*    一个 Note 占轨道内多个半块字符（▄/▀）或白底空格
          当 y 在两行中间（frac > 0.33 或 < -0.33）时：
            上行画 ▄（下半个块），下行画 ▀（上半个块）
            合起来就是一个完整的白色方块跨在两行之间
          当 y 接近某行中心时：
            直接在该行画白底空格（整格）*/
    int row = (int)(y + 0.5f);
    float frac = y - row;  // [-0.5, 0.5]

    // 边界检查
    int top = playing_windows[0][0] + 1;
    int bottom = playing_windows[1][0] - 1;
    if (if_judge_line_can_cover) bottom = playing_windows[1][0] - user_config.judge_line_position;

    // 判断是否在两行之间
    int between = (frac > 0.25f || frac < -0.25f);

    if (between) {
        // ▄ 在上行, ▀ 在下行
        int top_row = (frac > 0) ? row : row - 1;
        int bot_row = top_row + 1;
        if (top_row >= top && top_row <= bottom) {
            for (int i = 0; i < user_config.lane_padding; i++)
                draw_note_cell(top_row, (lane_line[0][1] + 1) + lane * (user_config.lane_padding + 1) + i, 0, color);
                // 参数说明:                左侧第一轨左上角                    第几轨               轨道内第几格
        }
        if (bot_row >= top && bot_row <= bottom) {
            for (int i = 0; i < user_config.lane_padding; i++)
                draw_note_cell(bot_row, (lane_line[0][1] + 1) + lane * (user_config.lane_padding + 1) + i, 1, color);
        }
    } else {
        // 单行内整格白底
        if (row < top || row > bottom) return;
        for (int i = 0; i < user_config.lane_padding; i++)
            draw_note_cell(row, (lane_line[0][1] + 1) + lane * (user_config.lane_padding + 1) + i, 2, color);
    }
    // ──────────────────────────────────────────────────────────────────
}



// 这个函数专门为了绘制hold的body，因为头尾在缓冲区Cell的顶层，覆盖在body上层，所以这里更改缓冲区Cell的底层（即bg_color）
//  参数:
//    lane     - 轨道号 (0 ~ keys-1)
//    head_row, tail_row - 头尾位置 (包含)
//    bg_color - 颜色
void draw_hold_body(int lane, int head_row, int tail_row, int color) {
    int top_bound = playing_windows[0][0] + 1;
    int bottom_bound = playing_windows[1][0] - user_config.judge_line_position;

    int lane_left_top = (lane_line[0][1] + 1) + lane * (user_config.lane_padding + 1);
    // 参数说明:            左侧第一轨左上角                    第几轨 
    for (int y = tail_row; y <= head_row; y++) {
        if (y < top_bound || y > bottom_bound) continue;
        for (int x = lane_left_top; x < lane_left_top + user_config.lane_padding; x++) {
            Cell *cell = &screen0.buffer[y][x];
            if (color >= 0) cell->bg_color = color;
        }
    }
}


// 4K 按键与键盘扫描码映射
static int keycode_for_lane(int lane) {
    switch (lane) {
        case 0: return user_config.key1_4k;
        case 1: return user_config.key2_4k;
        case 2: return user_config.key3_4k;
        case 3: return user_config.key4_4k;
    }
    return 0;
}


// 将按键字符转成虚拟键码（用于 GetAsyncKeyState）
// 'a'→0x41, 'z'→0x5A, '0'→0x30 等
static int char_to_vk(int ch) {
    if (ch >= 'a' && ch <= 'z') return ch - 'a' + 0x41;
    if (ch >= '0' && ch <= '9') return ch - '0' + 0x30;
    return ch;
}


// ─────────────────────── 逻辑更新函数 ───────────────────────
//  每帧由 update_playing_bar() 调用

// 渲染所有 Note
// 流程:
//   1. 清空轨道区域（避免上一帧的 note 残留）
//   2. 画判定线
//   3. 遍历 chart_notes 数组，对每个 Note:
//      a. 用 note_time_to_y() 算出当前位置
//      b. 如果超出可见范围（窗口顶-1 ~ 窗口底+1）则跳过
//      c. Tap: 直接画形状
//      d. Hold: 画head + 中间body + tail
// 显示 combo 大字体数字
void update_note(void) {
    int judge_y = playing_windows[1][0] - user_config.judge_line_position;
    int top_bound = playing_windows[0][0] + 1;
    int bot_bound = playing_windows[1][0] - 1;


    // 每帧先清空轨道区域（防止旧 note 残留） >>>如果要绘制背景特效就用screen_clear，这里删掉<<<
    for (int y = top_bound; y <= bot_bound; y++)
        screen_clear_line_range(&screen0, y, playing_windows[0][1] + 1, playing_windows[1][1] - 1);

    // 画轨道分隔线
    int keys = chart_info.keys;
    for (int y = playing_windows[0][0] + 1; y <= playing_windows[1][0] - 1; y++) {
        for (int key = 0; key <= keys; key++) {
            screen_set_cell(&screen0, y, lane_line[0][1] + key * (user_config.lane_padding + 1), "│", 15, COLOR_NONE);  // 灰字透明底
        }
    }

    // 画判定线
    for (int x = lane_line[0][1]; x <= lane_line[1][1]; x++)
        screen_set_cell(&screen0, judge_y, x, "─", 15, COLOR_NONE);

    if (chart_notes == NULL || chart_note_count == 0) return;


    int current_ms = get_game_time_ms();
    // 处理+渲染每个 Note
    for (int i = 0; i < chart_note_count; i++) {
        // 读取 Note
        Note *n = &chart_notes[i];

        // Note 类型判定
        /* ---------- Tap ---------- */
        if (n->type == NOTE_TAP) {
            // 忽略已击打 Tap
            if (n->hit) continue;

            // MISS 检测：超过 BAD 窗口还没被按
            if (current_ms - n->start_time > JUDGE_BAD_WINDOW) {
                n->hit = 1;
                n->hit_time = current_ms;
                score_miss++;
                combo = 0;
            }

            // 如果 Tap 越界，忽略
            float y = note_time_to_y(n->start_time, judge_y);
            if (y < top_bound - 1 || y > bot_bound + 1) continue;

            // 渲染 Tap
            draw_note_shape(n->lane, y, 15, 0);

        /* ---------- Hold ---------- */
        } else {
            // MISS 检测：头部超过 BAD 窗口还没被按
            int diff = current_ms - n->start_time;
            if (!n->hit && diff > JUDGE_BAD_WINDOW) {
                n->hit = 1;
                n->held = 2;
                n->hit_time = current_ms;
                score_miss++;
                combo = 0;
            }

            // 计算 hold 头尾
            float head_y = note_time_to_y(n->start_time, judge_y);
            float tail_y = note_time_to_y(n->end_time, judge_y);

            // 忽略区域外的 Hold(头部在边框上方、或尾部在判定线下方)
            if ((head_y <= top_bound - 1) && (tail_y >= judge_y + 1)) continue;

            // 长按
            int vk = char_to_vk(keycode_for_lane(n->lane));
            int key_down = (GetAsyncKeyState(vk) & 0x8000);

            // 按住的 Hold 尾部过了 BAD 区间，自动结算
            int release_diff = current_ms - n->end_time;  // 松手时差
            if (n->hit && (n->held == 1) && (release_diff > JUDGE_BAD_WINDOW)) {
                n->held = 2;
                score_perfect++;
                combo++;
                if (combo > max_combo) max_combo = combo;
            }
            // 按住的 Hold 松手了 -> 判定尾部
            else if (n->hit && (n->held == 1) && !key_down) {
                n->held = 2;

                int release_diff = n->end_time - current_ms;  // 松手时差
                // 松手miss了
                if (release_diff >= JUDGE_BAD_WINDOW) {
                    score_miss++;
                    combo = 0;
                // 松手在判定区间内
                } else {
                    score_perfect++;
                    combo++;
                    if (combo > max_combo) max_combo = combo;
                }
            }


            // 取整行号，尾部在屏幕下方（Y 更大）
            int tail_row = (int)(tail_y + 1.0f);
            int head_row = (int)(head_y - 0.25f);

            /* ------------- 渲染 Hold ------------- */
            if (n->held != 2)  // 非miss的hold
                draw_hold_body(n->lane, head_row, tail_row, 7);  // body（把背景设置成浅灰色，需要单独操作缓冲区Cell）
            else  // miss的hold
                draw_hold_body(n->lane, head_row, tail_row, 8);  // body 灰色
            if (n->held != 2)  // 非miss的hold tail
                draw_note_shape(n->lane, tail_y, 15, 1);  // tail
            else  // miss的hold tail
                draw_note_shape(n->lane, tail_y, 7, 1);  // tail
            draw_note_shape(n->lane, head_y, 15, 1);  // head
        }
    }
}



// ──────────────────── 按键按下检测 ────────────────────────
// 每个轨道下一个要检测的 Note 索引（避免每次从头遍历所有 Note）
static int next_note_idx[4] = {0, 0, 0, 0};

// 重置索引
void reset_playing_state(void) {
    for (int i = 0; i < 4; i++)
        next_note_idx[i] = 0;
}

// 查找指定轨道上距离判定线最近且未打击的 Note
// 返回索引，没找到返回 -1
static int find_hittable_note(int lane) {
    int current_ms = get_game_time_ms();

    // 从 next_note_idx 开始往后找
    for (int i = next_note_idx[lane]; i < chart_note_count; i++) {
        Note *n = &chart_notes[i];
        if (n->lane != lane) continue;     // 不是这条轨道的 Note
        if (n->hit) continue;              // 击打过了

        int diff = n->start_time - current_ms;
        // 在判定线下方 BAD 范围外
        if (diff < -JUDGE_BAD_WINDOW) {
            // 太远了
            next_note_idx[lane] = i + 1;
            continue;
        }

        // 在 BAD 范围内
        if (diff <= JUDGE_BAD_WINDOW) {
            next_note_idx[lane] = i + 1;   // 下次从下一个开始
            return i;
        }

        // diff > BAD_WINDOW -> Note 还没到，跳出
        break;
    }
    return -1;
}


// 根据时间差返回判定等级
static void tap_calc_judge(int hit_ms) {
    int abs_diff = (hit_ms < 0) ? -hit_ms : hit_ms;
    if      (abs_diff <= JUDGE_PERFECT_WINDOW)
    {
        tap_sound_effect();  // 打击音效
        score_perfect++; combo++;
    }
    else if (abs_diff <= JUDGE_GOOD_WINDOW)
    {
        tap_sound_effect();  // 打击音效
        score_good++;    combo++;
        if (hit_ms > 0) {
            score_good_fast++;
            record_fast_late(0);  // fast
        }
        else {
            score_good_late++;
            record_fast_late(1);  // late
        }
    }
    else if (abs_diff <= JUDGE_BAD_WINDOW)
    {
        tap_sound_effect();  // 打击音效
        score_bad++;     combo++;
        if (hit_ms > 0) {
            score_bad_fast++;
            record_fast_late(0);  // fast
        }
        else {
            score_bad_late++;
            record_fast_late(1);  // late
        }
    }
    // JUDGE_MISS
    else
    {
        score_miss++; combo=0;
    }
}


// 打击指定轨道的 Note (包括Hold头)
static void hit_lane(int lane) {
    int idx = find_hittable_note(lane);  // 查找最近可击打 Note
    if (idx < 0) return;  // 没有可打击的 Note

    int current_ms = get_game_time_ms();
    Note *n = &chart_notes[idx];

    n->hit = 1;
    n->hit_time = current_ms;

    // 头部判定
    int diff = n->start_time - current_ms;
    tap_calc_judge(diff);

    // Hold 头部按下后标记 held
    if (n->type == NOTE_HOLD) {
        n->held = 1;
    }

    if (combo > max_combo) max_combo = combo;
}



// 绘制画面&计算
void update_playing_ui(void) {
    // header第1行
    char line0[150];    if (strcmp(user_config.language, "en_us") == 0)
        sprintf(line0, "%s    Difficulty: %.2f⭐", chart_names[selected_chart_num], chart_info.star_rating);
    else if (strcmp(user_config.language, "zh_cn") == 0)
        sprintf(line0, "%s    难度: %.2f⭐", chart_names[selected_chart_num], chart_info.star_rating);
    screen_display_text(&screen0, 0, 0, line0, 219, COLOR_NONE);  // 粉字透明底

    // header第2行
    if (strcmp(user_config.language, "en_us") == 0)
        sprintf(line0, "audio=%s%s.mp3", chart_full_path, chart_names[selected_chart_num]);
    else if (strcmp(user_config.language, "zh_cn") == 0)
        sprintf(line0, "音频=%s%s.mp3", chart_full_path, chart_names[selected_chart_num]);
    screen_display_text(&screen0, 1, 0, line0, 15, COLOR_NONE);  // 白字透明底

    // header&footer直线
    for(int i = 0; i < user_config.width; i++) {
        screen_set_cell(&screen0, 2, i, "─", 15, COLOR_NONE);  // 白字透明底
        screen_set_cell(&screen0, user_config.height - 2, i, "─", 15, COLOR_NONE);  // 白字透明底
    }

    // footer第1行
    char footer[256];
    if (strcmp(user_config.language, "en_us") == 0)
        sprintf(footer, "PERFECT:%d  GOOD:%d  BAD:%d  MISS:%d  MAX_COMBO:%d   judge: a-0/b-0   (esc) Pause Menu",
                score_perfect, score_good, score_bad, score_miss, max_combo);  // 白字透明底
    else if (strcmp(user_config.language, "zh_cn") == 0)
        sprintf(footer, "PERFECT:%d  GOOD:%d  BAD:%d  MISS:%d  MAX_COMBO:%d   判定: a-0/b-0   (esc) 暂停菜单",
                score_perfect, score_good, score_bad, score_miss, max_combo);  // 白字透明底
    screen_display_text(&screen0, user_config.height - 1, 0, footer, 15, COLOR_NONE);


    // 画窗口边框
    screen_draw_frame(&screen0, playing_windows, " Live lanes ", 219);

    // 渲染下落 Note
    update_note();

    // 快慢指示器
    draw_fast_late();

    // combo提示
    draw_combo();

    // 分数提示
    draw_score();

    // fps指示器
    if (user_config.show_fps) {
        fps_display();
    }

    // 转场
    if (play_transition_animation) {transition_animation_fade_out(); play_transition_animation = 0;}

    render(&screen0, 1);

    // 结束退出
    Note *last = &chart_notes[chart_note_count - 1];
    int song_end = (last->type == NOTE_HOLD) ? last->end_time : last->start_time;
    if (get_game_time_ms() - song_end > 2000) {
        // 释放谱面
        free(chart_notes);
        chart_notes = NULL;

        // 转场
        play_transition_animation = 1;
        transition_animation_fade_in(); SLEEP_MS(600);

        game_state = STATE_RESULT;
    }
}



// 打歌页面的输入处理
void handle_playing_input(void) {
    if (_kbhit())
    {
        int key = _getch();
        // 打击检测
        if (key == user_config.key1_4k) hit_lane(0);
        if (key == user_config.key2_4k) hit_lane(1);
        if (key == user_config.key3_4k) hit_lane(2);
        if (key == user_config.key4_4k) hit_lane(3);
        // 按 Esc 暂停
        if (key == 27) {
            // 释放谱面
            free(chart_notes);
            chart_notes = NULL;

            audio_exit();

            // 转场
            play_transition_animation = 1;
            transition_animation_fade_in(); SLEEP_MS(600);

            game_state = STATE_SONG_SELECT;
        }
    }
}