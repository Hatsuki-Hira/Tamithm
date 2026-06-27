#pragma once

#include "../global.h"



void init_terminal(void);
void reset_terminal(void);
void get_terminal_size(int *width, int *height);
void screen_init(Screen *screen);
void screen_free(Screen *screen);



void screen_set_cell(
    Screen *screen,
    int y,
    int x,
    const char *ch,
    int fg_color,
    int bg_color
);



// 设置多个字符（自动换行，支持 UTF-8）
void screen_display_text(
    Screen *screen,
    int y,
    int x,
    const char *ch,
    int fg_color,
    int bg_color
);

// 设置多个字符（超出 max_x 自动换行，支持 UTF-8）
void screen_display_text_wrapped(
    Screen *screen,
    int y,
    int x,
    const char *ch,
    int max_x,
    int fg_color,
    int bg_color
);



// 清空屏幕缓冲区（全设为透明空格）
void screen_clear(Screen *screen);

// 清空某行的指定列范围（从 x1 到 x2，含 x2），填充透明空格
void screen_clear_line_range(Screen *screen, int y, int x1, int x2);

// 绘制窗口边框（可指定窗口标题和前景颜色）
void screen_draw_frame(Screen *screen, int position[2][2], const char *title, int fg_color);



void transition_animation_fade_in(void);
void transition_animation_fade_out(void);



void render(Screen *screen, int frame_waiting);
