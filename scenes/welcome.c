#include <stdio.h>

#include "../global.h"
#include "scene.h"



// 初始化：全屏幕 '#'
void update_welcome_ui(void) {
    for (int y = 0; y < user_config.height; y++)
    {
        for (int x = 0; x < user_config.width; x++)
        {
            screen.buffer[y][x] = "\033[47m \033[0m";
        }
    }
    render();
    SLEEP_MS(1000);

    render();
}

