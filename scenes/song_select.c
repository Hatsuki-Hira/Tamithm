#include <stdio.h>
#include <string.h>
#include <conio.h>

#include "../global.h"
#include "../data/renderer.h"
#include "../data/osu_compatible.h"
#include "scenes.h"



// 按键捕获模式
static int key_capture_mode = 0;



// 获取谱面信息
int get_charts_info(void) {
    char filepath[512];
    sprintf(filepath, "%s%s.osu", chart_full_path, chart_names[selected_chart_num]);
    FILE *fp = fopen(filepath, "r");
    if(fp == NULL)
        return -1;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        // 去掉末尾换行符
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

        if (strncmp(line, "TitleUnicode:", 13) == 0)
            strcpy(chart_info.title, line + 13);
        else if (strncmp(line, "Creator:", 8) == 0)
            strcpy(chart_info.creator, line + 8);
        else if (strncmp(line, "Version:", 8) == 0)
            strcpy(chart_info.difficulty, line + 8);
        else if (strncmp(line, "CircleSize:", 11) == 0)
            chart_info.keys = atoi(line + 11);
    }

    fclose(fp);
    return 0;
}



// 窗口坐标（全局变量定义）
int song_select_windows_menu[2][2];
int song_select_windows_charts[2][2];
int song_select_windows_preview[2][2];
int song_select_windows_details[2][2];

int song_select_windows_settings[2][2];

// 初始化窗口坐标
// 每个窗口用 [左上x, 左上y, 右下x, 右下y] 表示
void init_song_select_windows(void) {
    song_select_windows_menu[0][0] = 3;  // 左上y
    song_select_windows_menu[0][1] = 0;  // 左上x
    song_select_windows_menu[1][0] = user_config.height - 3;            // 右下y
    song_select_windows_menu[1][1] = (int)(user_config.width * 0.1);  // 右下x

    song_select_windows_charts[0][0] = 3;
    song_select_windows_charts[0][1] = (int)(user_config.width * 0.1) + 1;
    song_select_windows_charts[1][0] = user_config.height - 3;
    song_select_windows_charts[1][1] = (int)(user_config.width * 0.625);

    song_select_windows_preview[0][0] = 3;
    song_select_windows_preview[0][1] = (int)(user_config.width * 0.625) + 1;
    song_select_windows_preview[1][0] = (int)(user_config.height * 0.65);
    song_select_windows_preview[1][1] = user_config.width - 1;

    song_select_windows_details[0][0] = (int)(user_config.height * 0.65) + 1;
    song_select_windows_details[0][1] = (int)(user_config.width * 0.625) + 1;
    song_select_windows_details[1][0] = user_config.height - 3;
    song_select_windows_details[1][1] = user_config.width - 1;

    song_select_windows_settings[0][0] = 3;
    song_select_windows_settings[0][1] = (int)(user_config.width * 0.1) + 1;
    song_select_windows_settings[1][0] = user_config.height - 3;
    song_select_windows_settings[1][1] = user_config.width - 1;
}



// 绘制上下信息栏
void screen_draw_bar(void) {
    // header第1行
    screen_display_text(0, 0, "Tamithm  ", 219, COLOR_NONE);  // 粉字透明底

    char line0[150];
    if (strcmp(user_config.language, "en_us") == 0) {
        // header第1行
        sprintf(line0, "Charts(num=%d)  root=%s", chart_count, chart_full_path);
        screen_display_text(0, 9, line0, 15, COLOR_NONE);  // 白字透明底

        // header第2行
        screen_display_text(1, 0, "by 羽月ひら  (tab) Menu Option | (enter) Enter play | (a) Auto play | (r) Rescan Charts", 15, COLOR_NONE);  // 白字透明底

        // footer第2行
        screen_display_text(user_config.height - 1, 0, "(tab) Menu Option | (enter) Enter play | (a) Auto play | (r) Rescan Charts", 15, COLOR_NONE);  // 白字透明底
    } else
    if (strcmp(user_config.language, "zh_cn") == 0) {
        // header第1行
        sprintf(line0, "谱面(%d个)  root=%s", chart_count, chart_full_path);
        screen_display_text(0, 9, line0, 15, COLOR_NONE);  // 白字透明底

        // header第2行
        screen_display_text(1, 0, "by 羽月ひら  (tab) 菜单选项 | (enter) 启动! | (a) 自动游玩 | (r) 重新扫描谱面", 15, COLOR_NONE);  // 白字透明底

        // footer第2行
        screen_display_text(user_config.height - 1, 0, "(tab) 菜单选项 | (enter) 启动! | (a) 自动游玩 | (r) 重新扫描谱面", 15, COLOR_NONE);  // 白字透明底
    }

    // header&footer第3行直线
    for(int i = 0; i < user_config.width; i++) {
        screen_set_cell(2, i, "─", 81, COLOR_NONE);  // 蓝字透明底
        screen_set_cell(user_config.height - 2, i, "─", 81, COLOR_NONE);  // 蓝字透明底
    }
}



// 绘制Menu列表（单选）
MenuRadioButton menu_radio_button = Charts;
void screen_draw_menu_list(int position[2][2]) {
    int y0 = position[0][0];
    int x0 = position[0][1];
    if (menu_radio_button == Charts) {
        screen_display_text(y0 + 1, x0 + 1, "> Charts", 11, COLOR_NONE);  // 黄字透明底
        screen_display_text(y0 + 2, x0 + 1, "  Settings", 15, COLOR_NONE);  // 白字透明底
    }
    else if (menu_radio_button == Settings) {
        screen_display_text(y0 + 1, x0 + 1, "  Charts", 15, COLOR_NONE);  // 白字透明底
        screen_display_text(y0 + 2, x0 + 1, "> Settings", 11, COLOR_NONE);  // 黄字透明底
    }
}



// 绘制歌曲列表
void screen_draw_charts_list(int position[2][2]) {
    char num_output[4];
    for (int y = position[0][0] + 1; y <= position[1][0] - 1; y++) {
        int num = y - (position[0][0] + 1);
        // 不超过最大曲数
        if (num < chart_count) {
            sprintf(num_output, "%d ", num + 1);
            if (num == selected_chart_num) {
                // 黄字透明底
                screen_display_text(y, position[0][1] + 1, "> ", 11, COLOR_NONE);
                screen_display_text(y, position[0][1] + 3, num_output, 11, COLOR_NONE);
                screen_display_text(y, position[0][1] + 6, chart_names[num], 11, COLOR_NONE);
            } else {
                // 白字透明底
                screen_display_text(y, position[0][1] + 1, "  ", 15, COLOR_NONE);
                screen_display_text(y, position[0][1] + 3, num_output, 15, COLOR_NONE);
                screen_display_text(y, position[0][1] + 6, chart_names[num], 15, COLOR_NONE);
            }
        } else continue;
    }
}



// 绘制预览信息
void screen_draw_preview(int position[2][2]) {
    // 先清空预览窗口内容区域，防止文字残留
    for (int y = position[0][0] + 1; y < position[1][0]; y++) {
        screen_clear_line_range(y, position[0][1] + 1, position[1][1] - 1);
    }

    char line[256];
    sprintf(line, "Title: %s", chart_info.title);
    screen_display_text_wrapped(position[0][0] + 1, position[0][1] + 2, line, position[1][1], 15, COLOR_NONE);
    sprintf(line, "Creator: %s", chart_info.creator);
    screen_display_text_wrapped(position[0][0] + 2, position[0][1] + 2, line, position[1][1], 15, COLOR_NONE);
    sprintf(line, "Difficulty: %s", chart_info.difficulty);
    screen_display_text_wrapped(position[0][0] + 3, position[0][1] + 2, line, position[1][1], 15, COLOR_NONE);
    sprintf(line, "Keys: %d", chart_info.keys);
    screen_display_text_wrapped(position[0][0] + 4, position[0][1] + 2, line, position[1][1], 15, COLOR_NONE);
}



// 绘制详情界面
void screen_draw_details(int position[2][2]) {
    char line[256];
    sprintf(line, "Charts Compatible Mode: Osu!");
    screen_display_text_wrapped(position[0][0] + 1, position[0][1] + 2, line, position[1][1] - 1, 15, COLOR_NONE);
}


// 每个设置选项对应的页面行数
static const int setting_line_map[] = {
    2,   // 0: Fps
    3,   // 1: Language
    4,   // 2: Note Speed
    5,   // 3: Note Width (lane_padding)
    6,   // 4: Judge Line Position
    10,  // 5: key1
    11,  // 6: key2
    12,  // 7: key3
    13,  // 8: key4
};
// 选项总个数
int SETTING_TOTAL_COUNT = 9;

// 绘制设置界面language=en_us
void screen_draw_settings(int position[2][2]) {
    char line[14][256] = {" "};
    int middle_x;  // 居中后左上角x坐标
    if (strcmp(user_config.language, "en_us") == 0) {
        sprintf(line[0],  "---------------[general]---------------");
        sprintf(line[2],  "                Fps   %d", user_config.fps);
        sprintf(line[3],  "           Language   %s", user_config.language);
        sprintf(line[4],  "         Note Speed   %dms", user_config.note_speed);
        sprintf(line[5],  "         Note Width   %d", user_config.lane_padding);
        sprintf(line[6],  "Judge Line Position   %d", user_config.judge_line_position);
        sprintf(line[8],  "---------------[4k Mode]---------------");
        sprintf(line[9],  "            <Enter to edit>");
        sprintf(line[10], "               key1   %s", (key_capture_mode && selected_setting_num == 5) ? "<Press any key to bind...>" : (char[]){user_config.key1_4k, '\0'});
        sprintf(line[11], "               key2   %s", (key_capture_mode && selected_setting_num == 6) ? "<Press any key to bind...>" : (char[]){user_config.key2_4k, '\0'});
        sprintf(line[12], "               key3   %s", (key_capture_mode && selected_setting_num == 7) ? "<Press any key to bind...>" : (char[]){user_config.key3_4k, '\0'});
        sprintf(line[13], "               key4   %s", (key_capture_mode && selected_setting_num == 8) ? "<Press any key to bind...>" : (char[]){user_config.key4_4k, '\0'});
        middle_x = (position[1][1] + position[0][1]) / 2 - 20;
    } else
    if (strcmp(user_config.language, "zh_cn") == 0) {
        sprintf(line[0],  "-------[一般项]-------");
        sprintf(line[2],  "      帧数   %d", user_config.fps);
        sprintf(line[3],  "      语言   %s", user_config.language);
        sprintf(line[4],  "      流速   %dms", user_config.note_speed);
        sprintf(line[5],  "  音符宽度   %d", user_config.lane_padding);
        sprintf(line[6],  "判定线高度   %d", user_config.judge_line_position);
        sprintf(line[8],  "-------[4k模式]-------");
        sprintf(line[9],  "     <Enter以编辑>");
        sprintf(line[10], "      key1   %s", (key_capture_mode && selected_setting_num == 5) ? "<按下按键绑定...>" : (char[]){user_config.key1_4k, '\0'});
        sprintf(line[11], "      key2   %s", (key_capture_mode && selected_setting_num == 6) ? "<按下按键绑定...>" : (char[]){user_config.key2_4k, '\0'});
        sprintf(line[12], "      key3   %s", (key_capture_mode && selected_setting_num == 7) ? "<按下按键绑定...>" : (char[]){user_config.key3_4k, '\0'});
        sprintf(line[13], "      key4   %s", (key_capture_mode && selected_setting_num == 8) ? "<按下按键绑定...>" : (char[]){user_config.key4_4k, '\0'});
        middle_x = (position[1][1] + position[0][1]) / 2 - 11;
    }

    for (int i = 0; i < 14; i++) {
        // 高亮当前选中的设置项
        int color = 15;  // 白色
        if (i == setting_line_map[selected_setting_num]) {
            color = 219;  // 粉色高亮
        }
        screen_display_text(position[0][0] + i + 2, middle_x, line[i], color, COLOR_NONE);
    }
}



// 绘制header和footer
void update_song_select_ui(void) {
    screen_draw_bar();

    // 绘制Menu窗口
    screen_draw_frame(song_select_windows_menu, " Menu ", 15);
    screen_draw_menu_list(song_select_windows_menu);

    if (menu_radio_button == Charts) {
        // 绘制选歌窗口
        screen_draw_frame(song_select_windows_charts, " Charts ", 15);
        screen_draw_charts_list(song_select_windows_charts);

        // 绘制预览信息窗口
        get_charts_info();  // 获取谱面预览信息
        screen_draw_frame(song_select_windows_preview, " Preview ", 15);
        screen_draw_preview(song_select_windows_preview);

        // 绘制详情信息窗口
        screen_draw_frame(song_select_windows_details, " Details ", 15);
        screen_draw_details(song_select_windows_details);
    }

    else if (menu_radio_button == Settings) {
        // 绘制设置窗口
        screen_draw_frame(song_select_windows_settings, " Settings ", 15);
        screen_draw_settings(song_select_windows_settings);
    }

    render(1);
}



// 变更当前设置项的值
// direction: -1 = 左(减小), 1 = 右(增加)
static void change_setting_value(int direction) {
    switch (selected_setting_num) {
        case 0: // Fps
            user_config.fps += direction * 10;
            if (user_config.fps < 30) user_config.fps = 30;
            if (user_config.fps > 200) user_config.fps = 200;
            break;
        case 1: // Language
            if (strcmp(user_config.language, "en_us") == 0)
                strcpy(user_config.language, "zh_cn");
            else
                strcpy(user_config.language, "en_us");
            break;
        case 2: // Note Speed
            user_config.note_speed += direction * 10;
            if (user_config.note_speed < 100) user_config.note_speed = 100;
            if (user_config.note_speed > 2000) user_config.note_speed = 2000;
            break;
        case 3: // Note Width (lane_padding)
            user_config.lane_padding += direction;
            if (user_config.lane_padding < 0) user_config.lane_padding = 0;
            if (user_config.lane_padding > 10) user_config.lane_padding = 10;
            break;
        case 4: // Judge Line Position
            user_config.judge_line_position += direction;
            if (user_config.judge_line_position < 0) user_config.judge_line_position = 0;
            if (user_config.judge_line_position > 100) user_config.judge_line_position = 100;
            break;
    }
}

// 变更当前设置项的键位
static void change_setting_key(void) {
    switch (selected_setting_num) {
        case 5: // key1
        case 6: // key2
        case 7: // key3
        case 8: // key4
            key_capture_mode = 1;
            break;
    }
}


// 选歌页面的输入处理
void handle_song_select_input(void) {
    if (_kbhit())
    {
        // 清屏
        screen_clear();

        int key = _getch();

        // 按键捕获模式：将下一次按键绑定到当前选中的键位
        if (key_capture_mode) {
            if (key == 0 || key == 0xE0) {
                key = _getch();  // 特殊键，取扫描码
            }
            switch (selected_setting_num) {
                case 5: user_config.key1_4k = (char)key; break;
                case 6: user_config.key2_4k = (char)key; break;
                case 7: user_config.key3_4k = (char)key; break;
                case 8: user_config.key4_4k = (char)key; break;
            }
            key_capture_mode = 0;
            return;
        }
    
        // Menu切换选项
        if (key == '\t') {
            if (menu_radio_button == Charts) menu_radio_button = Settings;
            else menu_radio_button = Charts;
        }
        if (key == 'r') {
            load_charts_osu();
        }
        // 更改设置内容
        if (menu_radio_button == Settings && key == 72) {  // 上
            if (selected_setting_num > 0) {
                selected_setting_num--;
            }
        }
        if (menu_radio_button == Settings && key == 80) {  // 下
            if (selected_setting_num < SETTING_TOTAL_COUNT - 1) {
                selected_setting_num++;
            }
        }
        if (menu_radio_button == Settings && key == 75) {  // 左
            change_setting_value(-1);
        }
        if (menu_radio_button == Settings && key == 77) {  // 右
            change_setting_value(+1);
        }
        if (menu_radio_button == Settings && key == 13) {  // Enter
            change_setting_key();
        }
        if (menu_radio_button == Settings) config_save();  // 保存配置
        // 方向键选歌
        if (menu_radio_button == Charts && key == 72) {  // 上
            if (selected_chart_num > 0) {
                selected_chart_num--;
            }
        }
        if (menu_radio_button == Charts && key == 80) {  // 下
            if (selected_chart_num < chart_count - 1) {
                selected_chart_num++;
            }
        }
        if (menu_radio_button == Charts && key == 72 || key == 80) {
            get_charts_info();
        }
        // 按 Enter 打歌
        if (menu_radio_button == Charts && key == 13) {
            game_state = STATE_PLAYING;

            // 加载谱面
            char filepath[512];
            sprintf(filepath, "%s%s.osu", chart_full_path, chart_names[selected_chart_num]);
            osu_load_notes(filepath, &chart_notes, &chart_note_count);
            /* -------------- 测试：输出所有 Note 到文件 --------------
            FILE *ftest = fopen("test/notes_output.txt", "w");
            if (ftest) {
                fprintf(ftest, "Total notes: %d\n\n", chart_note_count);
                for (int i = 0; i < chart_note_count; i++) {
                    Note *n = &chart_notes[i];
                    fprintf(ftest, "[%4d] lane=%d  type=%s  start=%d",
                            i, n->lane,
                            (n->type == NOTE_TAP) ? "TAP " : "HOLD",
                            n->start_time);
                    if (n->type == NOTE_HOLD)
                        fprintf(ftest, "  end=%d  duration=%d", n->end_time, n->end_time - n->start_time);
                    fprintf(ftest, "\n");
                }
                fclose(ftest);
            }
            ------------------------------------------------------*/
        }
        // 按 Esc 返回主菜单
        if (key == 27) {
            game_state = STATE_WELCOME;
        }
    }
}