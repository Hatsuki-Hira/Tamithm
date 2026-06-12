#pragma once



#ifdef _WIN32
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)// 时间暂停
#else
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

#include <time.h>  // clock_t, clock()



// 用户数据
typedef struct {
    int width;
    int height;

    int fps;
    char language[8];
    int note_speed;
    int lane_padding;  // 轨道左右内边距（列数）
    int music_offset;

    char key1_4k;
    char key2_4k;
    char key3_4k;
    char key4_4k;

    int judge_line_position;
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



// 选歌界面当前歌曲、设置项索引序号
extern int selected_setting_num;
extern int selected_chart_num;



// 字符缓冲区
// 颜色值范围 0~255（256色）
#define COLOR_NONE 257     // 设置成终端默认颜色
#define COLOR_INHERIT -1  // 不改变颜色，继承旧颜色
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


// config 保存配置
void config_save(void);


// -------------------------------------
//           谱面相关全局定义
// -------------------------------------
// 播放音频
void audio_exit(void);

#define MAX_CHARTS 128
#define CHART_NAME_MAX 256

extern int chart_count;  // 谱面数量
extern char chart_names[MAX_CHARTS][CHART_NAME_MAX];  // 谱面名字数组
extern char chart_full_path[256];  // 谱面目录绝对路径



// 加载铺面
void scan_charts_list(void);
// 加载谱面所有内容(info note star_rating)
void load_chart(void);

// 谱面信息
typedef struct
{
    char title_unicode[128];
    char artist_unicode[128];
    char creator[64];
    char difficulty_name[64];
    int keys;
    double star_rating;
    char audio_filename[128];
    char source[128];
} ChartInfo;
extern ChartInfo chart_info;

// Note 类型
typedef enum
{
    NOTE_TAP,
    NOTE_HOLD
} NoteType;

// Note 数据
typedef struct
{
    NoteType type;

    int lane;
    int start_time;
    int end_time;

    int hit;          // 0=未打击, 1=已打击
    int held;         // 0=未打击, 1=按住, 2=松手 (Tap忽略此项)
    int hit_time;     // 实际按下的时间
} Note;

extern Note *chart_notes;  // 当前加载的谱面 Note 数据
extern int chart_note_count;  // Note 数量统计

extern clock_t game_start_time;  // 游戏计时（clock_t 的起始值）v

// 全局判定参数（窗口，单位毫秒）
#define JUDGE_PERFECT_WINDOW 80   // ±80ms
#define JUDGE_GOOD_WINDOW    160  // ±160ms
#define JUDGE_BAD_WINDOW     180  // ±180ms

// 判定等级
typedef enum {
    JUDGE_NONE,
    JUDGE_MISS,
    JUDGE_BAD,
    JUDGE_GOOD,
    JUDGE_PERFECT
} JudgeRank;

// 统计
extern int score_perfect;
extern int score_good;
extern int score_bad;
extern int score_miss;
extern int combo;
extern int max_combo;