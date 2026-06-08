#include <stdio.h>
#include <string.h>
#include <conio.h>

#include "../global.h"
#include "../data/renderer.h"
#include "scenes.h"



// 窗口坐标（全局变量定义）
int song_select_windows_menu[2][2];
int song_select_windows_charts[2][2];
int song_select_windows_preview[2][2];
int song_select_windows_details[2][2];

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



// 绘制header和footer
void update_song_select_bar(void) {
    // header第1行
    screen_display_text(0, 0, "Tamithm  ", 219, COLOR_NONE);  // 粉字透明底

    char line0[150];
    if (strcmp(user_config.language, "en_us") == 0)
        sprintf(line0, "Charts(num=%d)  root=%s", chart_count, chart_full_path);
    else if (strcmp(user_config.language, "zh_cn") == 0)
        sprintf(line0, "谱面(%d个)  root=%s", chart_count, chart_full_path);
    screen_display_text(0, 9, line0, 15, COLOR_NONE);  // 白字透明底

    // header第2行
    if (strcmp(user_config.language, "en_us") == 0)
        screen_display_text(1, 0, "by 羽月ひら  (tab) Menu Option | (enter) Enter play | (a) Auto play | (r) Rescan", 15, COLOR_NONE);  // 白字透明底
    else if (strcmp(user_config.language, "zh_cn") == 0)
        screen_display_text(1, 0, "by 羽月ひら  (tab) 菜单选项 | (enter) 启动! | (a) 自动游玩 | (r) 重新扫描", 15, COLOR_NONE);  // 白字透明底

    // header&footer第3行直线
    for(int i = 0; i < user_config.width; i++) {
        screen_set_cell(2, i, "─", 14, COLOR_NONE);  // 蓝字透明底
        screen_set_cell(user_config.height - 2, i, "─", 14, COLOR_NONE);  // 蓝字透明底
    }

    // footer第2行
    if (strcmp(user_config.language, "en_us") == 0)
        screen_display_text(user_config.height - 1, 0, "(tab) Menu Option | (enter) Enter play | (a) Auto play | (r) Rescan", 15, COLOR_NONE);  // 白字透明底
    else if (strcmp(user_config.language, "zh_cn") == 0)
        screen_display_text(user_config.height - 1, 0, "(tab) 菜单选项 | (enter) 启动! | (a) 自动游玩 | (r) 重新扫描", 15, COLOR_NONE);  // 白字透明底


    // 画窗口边框
    screen_draw_frame(song_select_windows_menu, " Menu ");
    screen_draw_frame(song_select_windows_charts, " Charts ");
    screen_draw_frame(song_select_windows_preview, " Preview ");
    screen_draw_frame(song_select_windows_details, " Details ");

    // Menu窗口内容
    screen_draw_menu_list(song_select_windows_menu);

    // 选歌列表
    screen_draw_charts_list(song_select_windows_charts);

    render(1);
}




// 选歌页面的输入处理
void handle_song_select_input(void) {
    if (_kbhit())
    {
        int key = _getch();
        // Menu切换选项
        if (key == '\t') {
            if (menu_radio_button == Charts) menu_radio_button = Settings;
            else menu_radio_button = Charts;
        }
        if (key == 'r') {
            load_charts();
            screen_clear();
        }
        // 方向键选歌
        if (key == 72) {
            if (selected_chart_num > 0) {
                selected_chart_num--;
            }
        }
        if (key == 80) {
            if (selected_chart_num < chart_count - 1) {
                selected_chart_num++;
            }
        }
        // 按 Esc 返回主菜单
        if (key == 27) {
            screen_clear();
            game_state = STATE_WELCOME;
        }
    }
}