#pragma once



void init_terminal(void);
void reset_terminal(void);
void get_terminal_size(int *width, int *height);
void screen_init(void);
void screen_free(void);



void screen_set_cell(
    int y,
    int x,
    const char *ch,
    int fg_color,
    int bg_color
);



// 设置多个字符（自动换行，支持 UTF-8）
void screen_display_text(
    int y,
    int x,
    const char *ch,
    int fg_color,
    int bg_color
);

// 设置多个字符（超出 max_x 自动换行，支持 UTF-8）
void screen_display_text_wrapped(
    int y,
    int x,
    const char *ch,
    int max_x,
    int fg_color,
    int bg_color
);



// 清空屏幕缓冲区（全设为黑底空格）
void screen_clear(void);

// 清空某行的指定列范围（从 x1 到 x2，含 x2），填充透明空格
void screen_clear_line_range(int y, int x1, int x2);



// 绘制窗口边框（可指定窗口标题和前景颜色）
void screen_draw_frame(int position[2][2], const char *title, int fg_color);



// Menu列表（单选）状态机
typedef enum {
    Charts,
    Settings
} MenuRadioButton;
extern MenuRadioButton menu_radio_button;



void render(int frame_waiting);
