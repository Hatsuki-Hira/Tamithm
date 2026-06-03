#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>

#include "global.h"
#include "scenes/scene.h"



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
        csbi.srWindow.Top + 1;

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


// 移动光标到 (x, y) 坐标 (注意：ANSI从1开始计数)
void move_cursor(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1);
}


// 初始化屏幕缓冲区
void screen_init() {
    screen.buffer = (char ***)malloc(user_config.height * sizeof(char **));
    for (int i = 0; i < user_config.height; i++)
    {
        screen.buffer[i] = (char **)malloc(user_config.width * sizeof(char *));
        for (int j = 0; j < user_config.width; j++)
        {
            screen.buffer[i][j] = " ";
        }
    }
}


// 释放屏幕缓冲区
void screen_free() {
    for (int i = 0; i < user_config.height; i++)
    {
        free(screen.buffer[i]);
    }
    free(screen.buffer);
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
    screen_init();  // 初始化屏幕缓冲区
    game_state = STATE_WELCOME;  // 展示欢迎界面
    // ------------------------------


    // 主程序入口
    while(game_state != STATE_EXIT) {
        // 键盘输入
        if (_kbhit())
        {
            char key = _getch();
        }

        // 页面状态机
        switch(game_state)
        {
            case STATE_WELCOME:
                get_terminal_size(&user_config.width, &user_config.height);
                screen_init();
                update_welcome_ui();
                if (_kbhit())
                {
                    game_state = STATE_SONG_SELECT;
                }
                break;

            case STATE_SONG_SELECT:
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
