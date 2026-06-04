#include <stdio.h>
#include <time.h>

#include "../global.h"



void screen_set_cell(
    int y,  // y行
    int x,  // x列
    char ch,  // 字符
    int color  // 底色
)
{
    Cell *cell = &screen.buffer[y][x];

    cell->ch = ch;
    cell->color = color;
}


#define RENDER_BUFFER_SIZE 500000
static char render_buffer[RENDER_BUFFER_SIZE];


// 渲染
void render(void) {
//    for(int y = 0; y < user_config.height; y++)
//    {
//        fwrite(screen.buffer[y], 1, user_config.width, stdout);
//        if (y < user_config.height - 1)
//        {
//            putchar('\n');
//        }
//    }

    char *ptr = render_buffer;

    ptr += sprintf(ptr, "\033[H");

    for(int y = 0; y < user_config.height; y++)
    {
        for(int x = 0; x < user_config.width; x++)
        {
            Cell *cell = &screen.buffer[y][x];

            ptr += sprintf(
                ptr,
                "\033[%dm%c",
                cell->color,
                cell->ch
            );
        }

        *ptr++ = '\n';
    }

    ptr += sprintf(ptr, "\033[0m");

    fwrite(
        render_buffer,
        1,
        ptr - render_buffer,
        stdout
    );

    fflush(stdout); // 刷新标准输出缓冲区，让画面同时呈现
    SLEEP_MS(1000 / user_config.fps);   // 控制帧率
}
