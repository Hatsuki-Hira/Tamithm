#pragma once



#ifdef _WIN32
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)// 时间暂停
#else
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)
#endif



// 用户数据
typedef struct {
    int width;
    int height;

    int fps;
    char language[8];
    int note_speed;
    int judge_line_y;
    int music_offset;
} Config;
extern Config user_config;



// 游戏页面状态机
typedef enum {
    STATE_WELCOME,     // 欢迎动画
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
// 颜色值范围 0~255（256色），257 = 终端默认色
#define COLOR_NONE 257
typedef struct
{
    char ch[5];           // UTF-8 字符（最多4字节 + null终止符）
    unsigned int fg_color; // 字体颜色（256色值，256=默认）
    unsigned int bg_color; // 背景色（256色值，256=默认）

} Cell;



typedef struct
{
    Cell **buffer;
} Screen;
extern Screen screen;



// 谱面相关
#define MAX_CHARTS 128
#define CHART_NAME_MAX 256

extern int chart_count;  // 谱面数量
extern char chart_names[MAX_CHARTS][CHART_NAME_MAX];  // 谱面名字数组
extern char chart_full_path[256];  // 谱面目录绝对路径
extern int selected_chart_num;  // 选歌界面当前歌曲序号

void load_charts(void);

typedef struct
{
    char title[128];
    char creator[64];
    char difficulty[64];

    int keys;
} ChartInfo;
extern ChartInfo chart_info;

typedef enum
{
    NOTE_TAP,
    NOTE_HOLD;
} NoteType;

typedef struct
{
    NoteType type;

    int lane;

    int start_time;
    int end_time;
} Note;