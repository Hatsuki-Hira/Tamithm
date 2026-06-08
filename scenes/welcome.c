#include <stdio.h>
#include <conio.h>

#include "../global.h"
#include "../data/renderer.h"
#include "../data/ascii_letters.h"



// 开屏游戏名
void update_welcome_ui(void) {
    display_text(user_config.width * 0.4, user_config.height * 0.4, "Tamithm", 0, 15);
    render(1);
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
            screen_clear();
            render(0);
            SLEEP_MS(100);
            game_state = STATE_SONG_SELECT;
        }
    }
}