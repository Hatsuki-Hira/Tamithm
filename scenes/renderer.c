#include <stdio.h>
#include <time.h>

#include "../global.h"



// 渲染
void render(void) {
    for(int y = 0; y < user_config.height; y++)
    {
        for (int x = 0; x < user_config.width; x++)
        {
            fputs(screen.buffer[y][x], stdout);
        }
        if (y < user_config.height - 1)
        {
            putchar('\n');
        }
    }
    
    fflush(stdout); // 刷新标准输出缓冲区，让画面同时呈现
    SLEEP_MS(1000 / user_config.fps);   // 控制帧率
}
