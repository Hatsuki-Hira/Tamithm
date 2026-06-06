#pragma once


void screen_set_cell(
    int y,
    int x,
    char ch,
    int color
);

// 清空屏幕缓冲区（全设为黑底空格）
void screen_clear(void);

void render(int frame_waiting);
