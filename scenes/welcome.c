#include <stdio.h>
#include <conio.h>

#include "../global.h"
#include "../data/renderer.h"
#include "../data/ascii_letters.h"



// 开屏游戏名
void update_welcome_ui(void) {
    display_text(user_config.width * 0.5 - 15, user_config.height * 0.5 - 2, "Tamithm", 0, 15);
    screen_display_text(&screen0, user_config.height * 0.5 + 8, user_config.width * 0.5 - 8, "<Press any key to start>", 7, COLOR_NONE);

    // 转场
    if (play_transition_animation) {transition_animation_fade_out(); play_transition_animation = 0;}

    render(&screen0, 1);
}
                


// 标题页面的输入处理
void handle_welcome_input(void) {
    // 任意按键进入游戏
    if (_kbhit())
    {
        int key = _getch();
        // 按 Esc 退出游戏
        if (key == 27) {
            game_state = STATE_EXIT;
        }
        else {
            // 转场
            play_transition_animation = 1;
            transition_animation_fade_in(); SLEEP_MS(600);

            game_state = STATE_SONG_SELECT;
        }
    }
}