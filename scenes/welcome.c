#include <stdio.h>

#include "../global.h"
#include "../data/renderer.h"
#include "../data/ascii_letters.h"



// 开屏游戏名
void update_welcome_ui0(void) {
    display_text(user_config.width * 0.4, user_config.height * 0.4, "Tamithm", 107);
    render(0);
    SLEEP_MS(1000);
}


void update_welcome_ui1(void) {
    for(int i = 0; i < 10; i++)
    {
        for (int y = 0; y < user_config.height; y++)
        {
            for (int x = 0; x < user_config.width; x++)
            {
                // screen.buffer = "\033[47m \033[0m";
                screen_set_cell(y, x, ' ', 0);
            }
        }
        render(0);
        SLEEP_MS(300);
    }
}
