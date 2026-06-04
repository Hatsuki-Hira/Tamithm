#include <stdio.h>

#include "../global.h"
#include "../data/renderer.h"
#include "../data/ascii_letters.h"
#include "../picture2ascii/picture2ascii.h"



// 开屏游戏名
void update_welcome_ui0(void) {
    for(int i = 0; i < 10; i++)
    {
        for (int y = 0; y < user_config.height; y++)
        {
            for (int x = 0; x < user_config.width; x++)
            {
                // screen.buffer = "\033[47m \033[0m";
                screen_set_cell(y, x, ' ', 107);
            }
        }
        render(0);
        screen_resize_detect();
        SLEEP_MS(300);
    }
    // 动画
    animation_scan_x_easing(100, 0, 0, 1, easing_in_out_quad);
}
void update_welcome_ui1(void) {
    screen_resize_detect();
    // 显示 "Tamithm" 文本，位置在屏幕中心，背景色为 0（黑色）
    display_text(user_config.width * 0.4, user_config.height * 0.4, "Tamithm", 107);
    render(0);
}

void update_welcome_ui2(void) {
    animation_scan_x_easing(0, 100, 107, 1, easing_in_out_quad);
    animation_scan_x_easing(100, 0, 0, 1, easing_in_out_quad);
    load_image_to_screen("D:\\Home\\Programming\\c\\Tamithm\\picture2ascii\\YOUNITHM.png", 1, 1, 100, 50);
    render(0);
}

