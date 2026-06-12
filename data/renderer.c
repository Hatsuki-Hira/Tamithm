#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "../global.h"
#include "renderer.h"




// 清屏并隐藏光标
void init_terminal() {
    // 设置控制台输出为 UTF-8，确保制表符（┌─┐等）正确显示
    SetConsoleOutputCP(CP_UTF8);
    printf("\033[2J");    // 清屏
    printf("\033[?25l");   // 隐藏光标
    fflush(stdout);
}



// 恢复光标显示
void reset_terminal() {
    printf("\033[2J");    // 清屏
    printf("\033[?25h");  // 显示光标
    printf("\033[1;1H");  // 移动到左上角
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
        csbi.srWindow.Left;

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



// 初始化屏幕缓冲区
void screen_init() {
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

    screen_clear();  // 缓冲区全部写入空格
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



// 设置单个字符（支持 UTF-8）
void screen_set_cell(
    int y,  // y行
    int x,  // x列
    const char *ch,  // 字符串（UTF-8 字符，最多4字节）
    int fg_color,    // 字体颜色（256色值，-1 表示不改变）
    int bg_color     // 背景色（256色值，-1 表示不改变）
)
{
    // 边界检查：防止调整大小时越界写入导致崩溃
    if (y < 0 || y >= user_config.height ||
        x < 0 || x >= user_config.width)
        return;
    if (screen.buffer == NULL || screen.buffer[y] == NULL)
        return;

    Cell *cell = &screen.buffer[y][x];

    strncpy(cell->ch, ch, 4);
    cell->ch[4] = '\0';
    if (fg_color >= 0) cell->fg_color = fg_color;
    if (bg_color >= 0) cell->bg_color = bg_color;
}



// 设置多个字符（自动换行，支持 UTF-8）
void screen_display_text(
    int y,  // y行
    int x,  // x列
    const char *ch,  // 字符串
    int fg_color,    // 字体颜色
    int bg_color     // 背景色
)
{
    while (*ch != '\0') {
        // 遇到换行符就换行
        if (*ch == '\n') {
            y++;
            x = 0;
            ch++;
            continue;
        }

        // 计算当前 UTF-8 字符占几个字节（通过首字节判断）
        int len;
        if      ((*ch & 0xF8) == 0xF0) len = 4;  // 4字节字符
        else if ((*ch & 0xF0) == 0xE0) len = 3;  // 3字节字符（如 ┌、─、┐）
        else if ((*ch & 0xE0) == 0xC0) len = 2;  // 2字节字符
        else                           len = 1;  // ASCII

        // 构造单字符字符串 buf = [0xE2] [0x94] [0x8C] [\0] [\0]  ← "┌"
        char buf[5] = {0};
        strncpy(buf, ch, len);

        screen_set_cell(y, x, buf, fg_color, bg_color);

        // 全角字符（3字节及以上的 UTF-8）占 2 列
        // 在当前格子 x 写入字符，在下一个格子 x+1 写入空字符串 "\0" (替换空格)作为占位
        if (len >= 3) {
            screen_set_cell(y, x + 1, "\0", fg_color, bg_color);
            x += 2;
        } else {
            x++;
        }
        // -----------------------------------------------------------------------
        ch += len;
    }
}


// 设置多个字符（超出 max_x 自动换行，支持 UTF-8）
void screen_display_text_wrapped(
    int y,
    int x,
    const char *ch,
    int max_x,
    int fg_color,
    int bg_color
) {
    int start_x = x;
    while (*ch != '\0') {
        // 遇到换行符就换行
        if (*ch == '\n') {
            y++;
            x = start_x;
            ch++;
            continue;
        }

        // 计算当前 UTF-8 字符占几个字节
        int len;
        if      ((*ch & 0xF8) == 0xF0) len = 4;
        else if ((*ch & 0xF0) == 0xE0) len = 3;
        else if ((*ch & 0xE0) == 0xC0) len = 2;
        else                           len = 1;

        // 如果当前位置已到达或超过右边界，自动换行
        if (x >= max_x) {
            y++;
            x = start_x;
        }

        char buf[5] = {0};
        strncpy(buf, ch, len);
        screen_set_cell(y, x, buf, fg_color, bg_color);

        if (len >= 3) {
            screen_set_cell(y, x + 1, "\0", fg_color, bg_color);
            x += 2;
        } else {
            x++;
        }
        ch += len;
    }
}


// 清空屏幕缓冲区（全设为透明空格）
void screen_clear(void) {
    for (int y = 0; y < user_config.height; y++) {
        for (int x = 0; x < user_config.width; x++) {
            screen_set_cell(y, x, " ", 0, COLOR_NONE);
        }
    }
}

// 清空某行的指定列范围（从 x1 到 x2，含 x2），填充透明空格
void screen_clear_line_range(int y, int x1, int x2) {
    for (int x = x1; x <= x2; x++) {
        screen_set_cell(y, x, " ", 0, COLOR_NONE);
    }
}



// 绘制窗口边框
void screen_draw_frame(int position[2][2], const char *title, int fg_color) {
    for(int y = position[0][0]; y <= position[1][0]; y++) {
        // 第一行
        if (y == position[0][0])
        {
            // 边角
            screen_set_cell(y, position[0][1], "┌", fg_color, COLOR_NONE);
            screen_set_cell(y, position[1][1], "┐", fg_color, COLOR_NONE);

            // 先画标题，再画水平棱边
            if (title != NULL && title[0] != '\0') {
                screen_display_text(y, position[0][1] + 1, title, fg_color, COLOR_NONE);
                int title_end = position[0][1] + 1 + strlen(title);
                for(int x = title_end; x < position[1][1]; x++)
                    screen_set_cell(y, x, "─", fg_color, COLOR_NONE);
            } else {
                for(int x = position[0][1] + 1; x < position[1][1]; x++)
                    screen_set_cell(y, x, "─", fg_color, COLOR_NONE);
            }

            // 底边水平线
            for(int x = position[0][1] + 1; x < position[1][1]; x++)
                screen_set_cell(position[1][0], x, "─", fg_color, COLOR_NONE);
        } 
        // 最后一行
        else if (y == position[1][0])
        {
            // 边角
            screen_set_cell(y, position[0][1], "└", fg_color, COLOR_NONE);
            screen_set_cell(y, position[1][1], "┘", fg_color, COLOR_NONE);
        } 
        // 竖直棱边
        else {
            screen_set_cell(y, position[0][1], "│", fg_color, COLOR_NONE);
            screen_set_cell(y, position[1][1], "│", fg_color, COLOR_NONE);
        }
    }
}




// 如果渲染缓冲区快满了，中途 flush 到终端防止溢出
#define RENDER_BUFFER_SIZE 500000
static char render_buffer[RENDER_BUFFER_SIZE];

#define RENDER_FLUSH_THRESHOLD (RENDER_BUFFER_SIZE - 4096)
static void render_flush_if_needed(char **pptr) {
    if ((size_t)(*pptr - render_buffer) >= RENDER_FLUSH_THRESHOLD) {
        fwrite(render_buffer, 1, *pptr - render_buffer, stdout);
        *pptr = render_buffer;
    }
}



// 渲染
void render(int frame_waiting) {
//    for(int y = 0; y < user_config.height; y++)
//    {
//        fwrite(screen.buffer[y], 1, user_config.width, stdout);
//        if (y < user_config.height - 1)
//        {
//            putchar('\n');
//        }
//    }
    char *ptr = render_buffer;

    render_flush_if_needed(&ptr);
    ptr += sprintf(ptr, "\033[H");

    for(int y = 0; y < user_config.height; y++)
    {
        int current_fg = -1;
        int current_bg = -1;
        
        for(int x = 0; x < user_config.width; x++)
        {
            render_flush_if_needed(&ptr);

            Cell *cell = &screen.buffer[y][x];
            
            // 如果前景色或背景色改变，先 reset 再输出新的颜色码
            if(cell->fg_color != current_fg ||
               cell->bg_color != current_bg)
            {
                if(current_fg != -1 || current_bg != -1)
                {
                    ptr += sprintf(ptr, "\033[0m");
                }
                current_fg = cell->fg_color;
                current_bg = cell->bg_color;

                // 前景色转义
                if (current_fg == COLOR_NONE)
                    ptr += sprintf(ptr, "\033[39m");  // 恢复默认前景0
                else
                    ptr += sprintf(ptr, "\033[38;5;%dm", current_fg);

                // 背景色转义
                if (current_bg == COLOR_NONE)
                    ptr += sprintf(ptr, "\033[49m");  // 恢复默认默认背景
                else
                    ptr += sprintf(ptr, "\033[48;5;%dm", current_bg);
            }
            
            //*ptr++ = cell->ch;

            // 全角字符右半边的占位格（ch[0]=='\0'）直接跳过，不输出任何内容
            if (cell->ch[0] == '\0')
                continue;
            else
                ptr += sprintf(ptr, "%s", cell->ch);
            // -------------------------------------------
        }

        // 每行结束恢复默认颜色并换行
        render_flush_if_needed(&ptr);
        ptr += sprintf(ptr, "\033[0m\n");
    }

    fwrite(
        render_buffer,
        1,
        ptr - render_buffer,
        stdout
    );

    fflush(stdout); // 刷新标准输出缓冲区，让画面同时呈现
    if(frame_waiting == 1)
        SLEEP_MS(1000 / user_config.fps);   // 控制帧率
}
