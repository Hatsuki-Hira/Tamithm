#include <stdio.h>
#include <time.h>
#include <math.h>

#include "../global.h"



// 设置单个字符
void screen_set_cell(
    int y,  // y行
    int x,  // x列
    char ch,  // 字符
    int color  // 底色
)
{
    // 边界检查：防止调整大小时越界写入导致崩溃
    if (y < 0 || y >= user_config.height ||
        x < 0 || x >= user_config.width)
        return;
    if (screen.buffer == NULL || screen.buffer[y] == NULL)
        return;

    Cell *cell = &screen.buffer[y][x];

    cell->ch = ch;
    cell->color = color;
}



// 清空屏幕缓冲区（全设为黑底空格）
void screen_clear() {
    Cell empty = {' ', 0};
    for (int y = 0; y < user_config.height; y++) {
        for (int x = 0; x < user_config.width; x++) {
            screen_set_cell(y, x, ' ', 0);
        }
    }
}



// 渲染
#define RENDER_BUFFER_SIZE 500000
static char render_buffer[RENDER_BUFFER_SIZE];

// 如果渲染缓冲区快满了，中途 flush 到终端防止溢出
#define RENDER_FLUSH_THRESHOLD (RENDER_BUFFER_SIZE - 4096)
static void render_flush_if_needed(char **pptr) {
    if ((size_t)(*pptr - render_buffer) >= RENDER_FLUSH_THRESHOLD) {
        fwrite(render_buffer, 1, *pptr - render_buffer, stdout);
        *pptr = render_buffer;
    }
}

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
        int current_color = -1;
        
        for(int x = 0; x < user_config.width; x++)
        {
            render_flush_if_needed(&ptr);

            Cell *cell = &screen.buffer[y][x];
            
            // 如果颜色改变，先输出之前的字符
            if(cell->color != current_color)
            {
                if(current_color != -1)
                {
                    ptr += sprintf(ptr, "\033[0m");
                }
                current_color = cell->color;
                ptr += sprintf(ptr, "\033[%dm", current_color);
            }
            
            *ptr++ = cell->ch;
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
