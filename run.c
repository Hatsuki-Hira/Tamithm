#include <stdio.h>
#include <time.h>
#include <conio.h>

#include "global.h"
#include "scenes/scenes.h"



// 清屏并隐藏光标
void init_terminal() {
    printf("\033[2J");    // 清屏
    printf("\033[?25l");   // 隐藏光标
    fflush(stdout);
}


// 恢复光标显示
void reset_terminal() {
    printf("\033[?25h");  // 显示光标
    printf("\033[%d;1H\n", user_config.height + 1);  // 移动到屏幕下方
    fflush(stdout);
}


// 获取终端尺寸
void get_terminal_size(int *width, int *height)
{
#ifdef _WIN32

    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(
        GetStdHandle(STD_OUTPUT_HANDLE),
        &csbi
    );

    *width =
        csbi.srWindow.Right -
        csbi.srWindow.Left + 1;

    *height =
        csbi.srWindow.Bottom -
        csbi.srWindow.Top;  // 不+1，保留最下面一行防止自动滚屏

#else

    struct winsize ws;

    ioctl(
        STDOUT_FILENO,
        TIOCGWINSZ,
        &ws
    );

    *width = ws.ws_col;
    *height = ws.ws_row;

#endif
}


// 读取用户数据
Config user_config;
int config_init(void)
{
    FILE *fp = fopen("D:\\Home\\Programming\\c\\Tamithm\\userdata", "r");
    if(fp == NULL)
        return -1;

    fscanf(fp, "fps %d",
           &user_config.fps);
    fclose(fp);
    return 0;
}


// 清空屏幕缓冲区（全设为黑底空格）
void screen_clear() {
    Cell empty = {' ', 40};
    for (int y = 0; y < user_config.height; y++) {
        for (int x = 0; x < user_config.width; x++) {
            screen.buffer[y][x] = empty;
        }
    }
}


// 初始化屏幕缓冲区
void screen_init() {
// buffer单独存字符串时用的，已废弃
//    screen.buffer = (char **)malloc(user_config.height * sizeof(char *));
//    for (int i = 0; i < user_config.height; i++)
//    {
//        screen.buffer[i] = (char *)malloc(user_config.width * sizeof(char));
//    }
    screen.buffer =
        malloc(user_config.height * sizeof(Cell*));

    if (screen.buffer == NULL) {
        fprintf(stderr, "Error: screen buffer allocation failed\n");
        exit(1);
    }

    for(int y=0;y<user_config.height;y++)
    {
        screen.buffer[y] =
            malloc(user_config.width * sizeof(Cell));
        if (screen.buffer[y] == NULL) {
            fprintf(stderr, "Error: screen buffer row allocation failed\n");
            exit(1);
        }
    }

    screen_clear();  // ← 新分配的缓冲区全部清零
}


// 释放屏幕缓冲区
void screen_free() {
    if (screen.buffer == NULL) return;
    for (int i = 0; i < user_config.height; i++)
    {
        if (screen.buffer[i] != NULL)
            free(screen.buffer[i]);
    }
    free(screen.buffer);
    screen.buffer = NULL;  // 防止悬空指针
}


// 屏幕尺寸变化
int last_width = 0;
int last_height = 0;
void screen_resize_detect() {
    int new_width;
    int new_height;

    get_terminal_size(
        &new_width,
        &new_height
    );

    // 校验：防止缩放过程中返回无效或极端尺寸
    if (new_width < 10) new_width = 10;
    if (new_height < 5) new_height = 5;
    if (new_width > 500) new_width = 500;    // 防止极端值导致 malloc 失败
    if (new_height > 200) new_height = 200;

    if(new_width != last_width ||
       new_height != last_height)
    {
        if(screen.buffer != NULL)
            screen_free();

        user_config.width = new_width;
        user_config.height = new_height;

        screen_init();

        last_width = new_width;
        last_height = new_height;

        printf("\033[2J"); // 尺寸变化后彻底清屏
    }
}


// 主进程
Screen screen;
GameState game_state;
int main() {

    // ------------初始化------------
    int ret = config_init();  // 初始化用户设置
    if (ret == -1)
    {
        printf("Error: 无法读取userdata文件\n");
        return -1;
    }
    init_terminal();  // 初始化终端
    
    get_terminal_size(&user_config.width, &user_config.height);  // 获取终端尺寸
    last_width = user_config.width;    //初始化，防止首次误触发
    last_height = user_config.height;  //
    screen_init();  // 初始化屏幕缓冲区
    // 展示欢迎界面
    update_welcome_ui0();
    game_state = STATE_WELCOME;
    // ------------------------------


    // 主程序入口
    while(game_state != STATE_EXIT) {
        screen_resize_detect();
        // 页面状态机
        switch(game_state)
        {
            case STATE_WELCOME:
            update_welcome_ui1();
                if (_kbhit())  // 键盘输入
                {
                    update_welcome_ui2();
                    game_state = STATE_SONG_SELECT;
                }
                break;

            case STATE_SONG_SELECT:
                //screen_free();
                //get_terminal_size(&user_config.width, &user_config.height);
                //screen_init();
                break;

            case STATE_PLAYING:
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
