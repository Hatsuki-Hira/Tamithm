#pragma once


// 时间暂停
#ifdef _WIN32
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)
#endif


// 用户数据
typedef struct {
    int width;
    int height;
    int fps;
    int note_speed;
    int judge_line_y;
    int music_offset;
} Config;
extern Config user_config;


// 游戏页面状态机
typedef enum {
    STATE_WELCOME,     // 欢迎动画
    STATE_MENU,        // 主菜单
    STATE_SONG_SELECT, // 选歌
    STATE_SETTING,     // 设置界面
    STATE_LOADING,     // 加载谱面
    STATE_PLAYING,     // 游戏中
    STATE_PAUSE,       // 暂停
    STATE_RESULT,      // 结算
    STATE_EXIT         // 退出
} GameState;
extern GameState game_state;


// 字符缓冲区
typedef struct
{
    char ch;
    unsigned int color;

} Cell;
typedef struct
{
    Cell **buffer;
} Screen;
extern Screen screen;
