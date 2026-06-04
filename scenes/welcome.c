#include <stdio.h>

#include "../global.h"
#include "scene.h"



// 开屏
void update_welcome_ui(void) {
    for (int y = 0; y < user_config.height; y++)
    {
        for (int x = 0; x < user_config.width; x++)
        {
            // screen.buffer = "\033[47m \033[0m";
            screen_set_cell(y, x, ' ', 47);
        }
    }
    render();
    SLEEP_MS(1000);

    render();
}

