#include <stdio.h>
#include <time.h>
#include <conio.h>

#include "global.h"
#include "data/renderer.h"
#include "scenes/scenes.h"




// 谱面列表（全局变量定义）
int chart_count = 0;
char chart_names[MAX_CHARTS][CHART_NAME_MAX];
int selected_chart_num = 0;  // 选歌界面当前歌曲
ChartInfo chart_info;
Note *chart_notes = NULL;  // 加载谱面内note
int chart_note_count = 0;  // 谱面内note数量
clock_t game_start_time = 0;
// 游玩统计
int score_perfect = 0;
int score_good = 0;
int score_bad = 0;
int score_miss = 0;
int combo = 0;
int max_combo = 0;



// 扫描 charts 目录，加载所有 .osu 文件名
void load_charts(void) {
    WIN32_FIND_DATAW find_data;
    HANDLE hFind = FindFirstFileW(L"charts\\*.osu", &find_data);

    chart_count = 0;

    if (hFind == INVALID_HANDLE_VALUE)
        return;  // 目录为空或不存在

    do {
        // 将 UTF-16 文件名转换为 UTF-8
        int len_utf8 = WideCharToMultiByte(
            CP_UTF8, 0,
            find_data.cFileName, -1,
            NULL, 0, NULL, NULL
        );
        if (len_utf8 > 0) {
            WideCharToMultiByte(
                CP_UTF8, 0,
                find_data.cFileName, -1,
                chart_names[chart_count], len_utf8,
                NULL, NULL
            );
            // 去掉末尾的 ".osu" 扩展名
            size_t len = strlen(chart_names[chart_count]);
            if (len > 4 && strcmp(chart_names[chart_count] + len - 4, ".osu") == 0)
                chart_names[chart_count][len - 4] = '\0';
        }

        chart_count++;
    } while (FindNextFileW(hFind, &find_data) && chart_count < MAX_CHARTS);

    FindClose(hFind);
}



// 读取用户数据
Config user_config;
int config_init(void)
{
    FILE *fp = fopen("D:\\Home\\Programming\\c\\Tamithm\\userdata", "r");
    if(fp == NULL)
        return -1;

    fscanf(fp, "fps=%d\n", &user_config.fps);
    fscanf(fp, "language=%s\n", &user_config.language);
    fscanf(fp, "note_speed_ms=%d\n", &user_config.note_speed);
    fscanf(fp, "lane_padding=%d\n", &user_config.lane_padding);
    fscanf(fp, "key1_4k=%c\n", &user_config.key1_4k);
    fscanf(fp, "key2_4k=%c\n", &user_config.key2_4k);
    fscanf(fp, "key3_4k=%c\n", &user_config.key3_4k);
    fscanf(fp, "key4_4k=%c\n", &user_config.key4_4k);
    fscanf(fp, "judge_line_position=%d\n", &user_config.judge_line_position);

    fclose(fp);
    return 0;
}



char chart_full_path[256];
// 获取 charts 目录的绝对路径
void get_charts_path(char *out, int out_size) {
    char rel[64] = "charts\\";
    GetFullPathName(rel, out_size, out, NULL);
}



// 音频库
#define MINIAUDIO_IMPLEMENTATION
#include "data/miniaudio.h"


ma_engine engine;
ma_result result;
int audio_init(void)
{
    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        return -1;
    }
}

int audio_play(void)
{
    char filepath[512];
    sprintf(filepath, "%s%s.mp3", chart_full_path, chart_names[selected_chart_num]);
    ma_engine_play_sound(&engine, filepath, NULL);

    return 0;
}

void audio_exit(void)
{
    ma_engine_uninit(&engine);
}




// 主进程
Screen screen;
GameState game_state;
int main() {

    // ----------------初始化----------------
    int ret = config_init();  // 初始化用户设置
    if (ret == -1)
    {
        printf("Error: 无法读取userdata文件\n");
        return -1;
    }

    init_terminal();  // 初始化终端
    get_terminal_size(&user_config.width, &user_config.height);  // 获取终端尺寸
    screen_init();  // 初始化屏幕缓冲区
    init_song_select_windows();  //初始化选歌界面的布局
    init_playing_windows();  //初始化轨道的布局

    // 谱面初始化
    load_charts();  // 加载谱面列表
    get_charts_path(chart_full_path, sizeof(chart_full_path));
    // -----------------------------------

    // 展示欢迎界面
    game_state = STATE_WELCOME;


    // 主程序入口
    while(game_state != STATE_EXIT) {
        // 页面切换状态机
        switch(game_state)
        {
            case STATE_WELCOME:
                update_welcome_ui();
                handle_welcome_input();
                break;

            case STATE_SONG_SELECT:
                update_song_select_bar();
                handle_song_select_input();
                break;

            case STATE_PLAYING:
                // 初始化
                if (game_start_time == 0) {
                    reset_playing_state();  // 重置打击索引
                    for (int i = 0; i < chart_note_count; i++) {
                        chart_notes[i].hit = 0;
                        chart_notes[i].held = 0;
                    }
                    game_start_time = clock();  // 启动计时器
                    score_perfect = 0;
                    score_good = 0;
                    score_bad = 0;
                    score_miss = 0;
                    combo = 0;
                    max_combo = 0;
                    audio_init();
                    audio_play();  // 播放音频
                }
                // 游戏逻辑更新
                update_playing_bar();
                handle_playing_input();
                break;

            case STATE_PAUSE:
                break;

            case STATE_RESULT:
                break;
        }

    }

    screen_free();  // 释放屏幕缓冲区
    reset_terminal();  // 释放终端
    return 0;
}
