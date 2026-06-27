#include <stdio.h>
#include <string.h>
#include <conio.h>

#include "../global.h"
#include "../data/renderer.h"
#include "../data/osu_compatible.h"
#include "../data/rating_caculate.h"
#include "../data/audio_system.h"
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
            strcpy(chart_info.title_unicode, line + 13);
        else if (strncmp(line, "ArtistUnicode:", 14) == 0)
            strcpy(chart_info.artist_unicode, line + 14);
        else if (strncmp(line, "Creator:", 8) == 0)
            strcpy(chart_info.creator, line + 8);
        else if (strncmp(line, "Version:", 8) == 0)
            strcpy(chart_info.difficulty_name, line + 8);
        else if (strncmp(line, "CircleSize:", 11) == 0)
            chart_info.keys = atoi(line + 11);
        else if (strncmp(line, "AudioFilename:", 14) == 0)
            strcpy(chart_info.audio_filename, line + 14);
        else if (strncmp(line, "Source:", 7) == 0)
            strcpy(chart_info.source, line + 7);
    }

    fclose(fp);
    return 0;
}



// 加载谱面
void load_chart(void) {
    // 加载谱面
    char filepath[512];
    sprintf(filepath, "%s%s.osu", chart_full_path, chart_names[selected_chart_num]);
    osu_load_notes(filepath, &chart_notes, &chart_note_count);
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
    screen_display_text(&screen0, 0, 0, "Tamithm  ", 219, COLOR_NONE);  // 粉字透明底

    char line0[150];
    if (strcmp(user_config.language, "en_us") == 0) {
        // header第1行
        sprintf(line0, "Charts(num=%d)  root=%s", chart_count, chart_full_path);
        screen_display_text(&screen0, 0, 9, line0, 15, COLOR_NONE);  // 白字透明底

        // header第2行
        screen_display_text(&screen0, 1, 0, "by 羽月ひら~  (tab) Menu Option | (enter) Enter play | (a) Auto play | (r) Rescan Charts", 15, COLOR_NONE);  // 白字透明底

        // footer第2行
        screen_display_text(&screen0, user_config.height - 1, 0, "(tab) Menu Option | (enter) Enter play | (a) Auto play | (r) Rescan Charts", 15, COLOR_NONE);  // 白字透明底
    } else
    if (strcmp(user_config.language, "zh_cn") == 0) {
        // header第1行
        sprintf(line0, "谱面(%d个)  root=%s", chart_count, chart_full_path);
        screen_display_text(&screen0, 0, 9, line0, 15, COLOR_NONE);  // 白字透明底

        // header第2行
        screen_display_text(&screen0, 1, 0, "by 羽月ひら~  (tab) 菜单选项 | (enter) 启动! | (a) 自动游玩 | (r) 重新扫描谱面", 15, COLOR_NONE);  // 白字透明底

        // footer第2行
        screen_display_text(&screen0, user_config.height - 1, 0, "(tab) 菜单选项 | (enter) 启动! | (a) 自动游玩 | (r) 重新扫描谱面", 15, COLOR_NONE);  // 白字透明底
    }

    // header&footer第3行直线
    for(int i = 0; i < user_config.width; i++) {
        screen_set_cell(&screen0, 2, i, "─", 81, COLOR_NONE);  // 蓝字透明底
        screen_set_cell(&screen0, user_config.height - 2, i, "─", 81, COLOR_NONE);  // 蓝字透明底
    }
}



// 绘制Menu列表（单选）
MenuRadioButton menu_radio_button = Charts;
void screen_draw_menu_list(int position[2][2]) {
    int y0 = position[0][0];
    int x0 = position[0][1];
    if (menu_radio_button == Charts) {
        screen_display_text(&screen0, y0 + 1, x0 + 1, "> Charts", 11, COLOR_NONE);  // 黄字透明底
        screen_display_text(&screen0, y0 + 2, x0 + 1, "  Settings", 15, COLOR_NONE);  // 白字透明底
    }
    else if (menu_radio_button == Settings) {
        screen_display_text(&screen0, y0 + 1, x0 + 1, "  Charts", 15, COLOR_NONE);  // 白字透明底
        screen_display_text(&screen0, y0 + 2, x0 + 1, "> Settings", 11, COLOR_NONE);  // 黄字透明底
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
                screen_display_text(&screen0, y, position[0][1] + 1, "> ", 11, COLOR_NONE);
                screen_display_text(&screen0, y, position[0][1] + 3, num_output, 11, COLOR_NONE);
                screen_display_text(&screen0, y, position[0][1] + 6, chart_names[num], 11, COLOR_NONE);
            } else {
                // 白字透明底
                screen_display_text(&screen0, y, position[0][1] + 1, "  ", 15, COLOR_NONE);
                screen_display_text(&screen0, y, position[0][1] + 3, num_output, 15, COLOR_NONE);
                screen_display_text(&screen0, y, position[0][1] + 6, chart_names[num], 15, COLOR_NONE);
            }
        } else continue;
    }
}



// 绘制预览信息
void screen_draw_preview(int position[2][2]) {
    // 先清空预览窗口内容区域，防止文字残留
    for (int y = position[0][0] + 1; y < position[1][0]; y++) {
        screen_clear_line_range(&screen0, y, position[0][1] + 1, position[1][1] - 1);
    }

    char line[8][256];
    if (strcmp(user_config.language, "en_us") == 0) {
        sprintf(line[0], "Title: %s", chart_info.title_unicode);
        sprintf(line[1], "Artist: %s", chart_info.artist_unicode);
        sprintf(line[2], "Difficulty: %.2f⭐", chart_info.star_rating);
        sprintf(line[3], "--------------------");
        sprintf(line[4], "Chart Creator: %s", chart_info.creator);
        sprintf(line[5], "Level Name: %s", chart_info.difficulty_name);
        sprintf(line[6], "Keys: %d", chart_info.keys);
        sprintf(line[7], "Source: %s", chart_info.source);
    } else
    if (strcmp(user_config.language, "zh_cn") == 0) {
        sprintf(line[0], "曲名: %s", chart_info.title_unicode);
        sprintf(line[1], "曲师: %s", chart_info.artist_unicode);
        sprintf(line[2], "难度: %.2f⭐", chart_info.star_rating);
        sprintf(line[3], "--------------------");
        sprintf(line[4], "制谱师: %s", chart_info.creator);
        sprintf(line[5], "关卡名称: %s", chart_info.difficulty_name);
        sprintf(line[6], "键数: %d", chart_info.keys);
        sprintf(line[7], "歌曲来源: %s", chart_info.source);
    }
    for (int i = 0;i < 8; i++) {
        screen_display_text_wrapped(&screen0, position[0][0] + (i + 1), position[0][1] + 2, line[i], position[1][1], 15, COLOR_NONE);
    }
}



// 绘制详情界面
void screen_draw_details(int position[2][2]) {
    char line[256];
    if (strcmp(user_config.language, "en_us") == 0) sprintf(line, "Charts Compatible Mode: Rhythm Game Osu!");
    else if (strcmp(user_config.language, "zh_cn") == 0) sprintf(line, "谱面兼容模式: 音乐游戏Osu!");
    screen_display_text_wrapped(&screen0, position[0][0] + 1, position[0][1] + 2, line, position[1][1] - 1, 15, COLOR_NONE);
}


// 每个设置选项对应的页面行数
static const int setting_line_map[] = {
    2,   // 0: Language
    3,   // 1: Note Speed
    4,   // 2: Note Width (lane_padding)
    5,   // 3: Judge Line Position
    9,   // 4: Music Offset
    10,  // 5: Judge Offset
    14,  // 6: key1
    15,  // 7: key2
    16,  // 8: key3
    17,  // 9: key4
    21,  // 10: Fps Limit
    22,  // 11: Show Fps
};
// 选项总个数
int SETTING_TOTAL_COUNT = 12;

// 绘制设置界面language=en_us
void screen_draw_settings(int position[2][2]) {
    char line[30][256] = {" "};
    int middle_x;  // 居中后左上角x坐标
    if (strcmp(user_config.language, "en_us") == 0) {
        sprintf(line[0],  "---------------[general]---------------");
        sprintf(line[2],  "           Language   %s", user_config.language);
        sprintf(line[3],  "         Note Speed   %dms", user_config.note_speed);
        sprintf(line[4],  "         Note Width   %d", user_config.lane_padding);
        sprintf(line[5],  "Judge Line Position   %d", user_config.judge_line_position);
        sprintf(line[7],  "-------------[judge&music]-------------");
        sprintf(line[9],  "       Music Offset   %dms", user_config.music_offset);
        sprintf(line[10], "       Judge Offset   %dms", user_config.judge_offset);
        sprintf(line[12], "---------------[4k Mode]---------------");
        sprintf(line[13], "            <Enter to edit>");
        sprintf(line[14], "               key1   %s", (key_capture_mode && selected_setting_num == 6) ? "<Press any key to bind...>" : (char[]){user_config.key1_4k, '\0'});
        sprintf(line[15], "               key2   %s", (key_capture_mode && selected_setting_num == 7) ? "<Press any key to bind...>" : (char[]){user_config.key2_4k, '\0'});
        sprintf(line[16], "               key3   %s", (key_capture_mode && selected_setting_num == 8) ? "<Press any key to bind...>" : (char[]){user_config.key3_4k, '\0'});
        sprintf(line[17], "               key4   %s", (key_capture_mode && selected_setting_num == 9) ? "<Press any key to bind...>" : (char[]){user_config.key4_4k, '\0'});
        sprintf(line[19], "---------------[render]----------------");
        sprintf(line[21], "          Fps Limit   %d", user_config.fps);
        sprintf(line[22], "           Show Fps   %s", user_config.show_fps? "True" : "False");
        middle_x = (position[1][1] + position[0][1]) / 2 - 20;
    } else
    if (strcmp(user_config.language, "zh_cn") == 0) {
        sprintf(line[0],  "-------[一般项]-------");
        sprintf(line[2],  "      语言   %s", user_config.language);
        sprintf(line[3],  "      流速   %dms", user_config.note_speed);
        sprintf(line[4],  "  音符宽度   %d", user_config.lane_padding);
        sprintf(line[5],  "判定线高度   %d", user_config.judge_line_position);
        sprintf(line[7],  "------[判定&音乐]------");
        sprintf(line[9],  "  音频延迟   %dms", user_config.music_offset);
        sprintf(line[10], "  视觉延迟   %dms", user_config.judge_offset);
        sprintf(line[12], "-------[4k模式]-------");
        sprintf(line[13], "    <Enter以编辑>");
        sprintf(line[14], "      key1   %s", (key_capture_mode && selected_setting_num == 6) ? "<按下按键绑定...>" : (char[]){user_config.key1_4k, '\0'});
        sprintf(line[15], "      key2   %s", (key_capture_mode && selected_setting_num == 7) ? "<按下按键绑定...>" : (char[]){user_config.key2_4k, '\0'});
        sprintf(line[16], "      key3   %s", (key_capture_mode && selected_setting_num == 8) ? "<按下按键绑定...>" : (char[]){user_config.key3_4k, '\0'});
        sprintf(line[17], "      key4   %s", (key_capture_mode && selected_setting_num == 9) ? "<按下按键绑定...>" : (char[]){user_config.key4_4k, '\0'});
        sprintf(line[19], "--------[渲染]--------");
        sprintf(line[21], "   Fps限制   %d", user_config.fps);
        sprintf(line[22], "   显示Fps   %s", user_config.show_fps? "开" : "关");
        middle_x = (position[1][1] + position[0][1]) / 2 - 11;
    }

    for (int i = 0; i < 23; i++) {
        // 高亮当前选中的设置项
        int color = 15;  // 白色
        if (i == setting_line_map[selected_setting_num]) {
            color = 219;  // 粉色高亮
        }
        screen_display_text(&screen0, position[0][0] + i + 2, middle_x, line[i], color, COLOR_NONE);
    }
}



// 绘制整个ui
void update_song_select_ui(void) {
    // 清屏
    screen_clear(&screen0);
    
    screen_draw_bar();

    // 绘制Menu窗口
    screen_draw_frame(&screen0, song_select_windows_menu, " Menu ", 15);
    screen_draw_menu_list(song_select_windows_menu);

    if (menu_radio_button == Charts) {
        // 绘制选歌窗口
        screen_draw_frame(&screen0, song_select_windows_charts, " Charts ", 15);
        screen_draw_charts_list(song_select_windows_charts);

        // 绘制预览信息窗口
        get_charts_info();  // 获取谱面预览信息
        screen_draw_frame(&screen0, song_select_windows_preview, " Preview ", 15);
        screen_draw_preview(song_select_windows_preview);

        // 绘制详情信息窗口
        screen_draw_frame(&screen0, song_select_windows_details, " Details ", 15);
        screen_draw_details(song_select_windows_details);
    }

    else if (menu_radio_button == Settings) {
        // 绘制设置窗口
        screen_draw_frame(&screen0, song_select_windows_settings, " Settings ", 15);
        screen_draw_settings(song_select_windows_settings);
    }

    if (user_config.show_fps) {
        fps_display();
    }

    // 转场
    if (play_transition_animation) {transition_animation_fade_out(); play_transition_animation = 0;}
    render(&screen0, 1);
}



// 变更当前设置项的值
// direction: -1 = 左(减小), 1 = 右(增加)
static void change_setting_value(int direction) {
    switch (selected_setting_num) {
        case 0: // Language
            if (strcmp(user_config.language, "en_us") == 0)
                strcpy(user_config.language, "zh_cn");
            else
                strcpy(user_config.language, "en_us");
            break;
        case 1: // Note Speed
            user_config.note_speed += direction * 10;
            if (user_config.note_speed < 100) user_config.note_speed = 100;
            if (user_config.note_speed > 2000) user_config.note_speed = 2000;
            break;
        case 2: // Note Width (lane_padding)
            user_config.lane_padding += direction;
            if (user_config.lane_padding < 0) user_config.lane_padding = 0;
            if (user_config.lane_padding > 10) user_config.lane_padding = 10;
            break;
        case 3: // Judge Line Position
            user_config.judge_line_position += direction;
            if (user_config.judge_line_position < 0) user_config.judge_line_position = 0;
            if (user_config.judge_line_position > 100) user_config.judge_line_position = 100;
            break;
        case 4: // Music Offset
            user_config.music_offset += direction;
            if (user_config.music_offset < -400) user_config.music_offset = -400;
            if (user_config.music_offset > 400) user_config.music_offset = 400;
            break;
        case 5: // Judge Offset
            user_config.judge_offset += direction;
            if (user_config.judge_offset < -400) user_config.judge_offset = -400;
            if (user_config.judge_offset > 400) user_config.judge_offset = 400;
            break;
        case 10: // Fps Limit
            user_config.fps += direction * 10;
            if (user_config.fps < 30) user_config.fps = 30;
            if (user_config.fps > 500) user_config.fps = 500;
            break;
        case 11: // Show Fps
            if (user_config.show_fps == 0) user_config.show_fps = 1;
            else user_config.show_fps = 0;
            break;
    }
}

// 变更当前设置项的键位
static void change_setting_key(void) {
    switch (selected_setting_num) {
        case 6: // key1
        case 7: // key2
        case 8: // key3
        case 9: // key4
            key_capture_mode = 1;
            break;
    }
}



// 获取谱面信息(包括难度)
void get_chart_info_all(void) {
    // 读取铺面简要信息
    get_charts_info();
    // 加载铺面
    load_chart();
    // 计算star_rating
    chart_info.star_rating = mania_calculate_rating(chart_notes, chart_note_count, chart_info.keys);
    // 释放谱面内存
    free(chart_notes);
}



// 选歌页面的输入处理
void handle_song_select_input(void) {
    if (_kbhit())
    {
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
            scan_charts_list();
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
            get_chart_info_all();
        }
        // 按 Enter 开始打歌!
        if (menu_radio_button == Charts && key == 13) {
            // 转场
            play_transition_animation = 1;
            transition_animation_fade_in(); SLEEP_MS(600);

            // 初始化游戏开始时间
            game_start_time = 0;

            // 加载谱面
            load_chart();
            game_state = STATE_PLAYING;

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
            // 转场
            play_transition_animation = 1;
            transition_animation_fade_in(); SLEEP_MS(600);
    
            game_state = STATE_WELCOME;
        }
    }
}