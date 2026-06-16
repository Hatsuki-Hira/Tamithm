#pragma once

// combo 数字大字体（0~9），每行3个字符宽，共3行
extern const char *combo_font[10][3][3];
extern const char *combo_logo[3][20];

// 结算等级字体（0~9），每行5个字符宽，共3行
extern const char *level_font[10][3][5];

// 显示单个大字体字符
// 参数：x, y - 左上角位置，ch - 字符，fg_color - 字体颜色，bg_color - 背景色
void display_char(int x, int y, char ch, int fg_color, int bg_color);

// 显示字符串
// 参数：x, y - 左上角位置，text - 字符串，fg_color - 字体颜色，bg_color - 背景色
void display_text(int x, int y, const char *text, int fg_color, int bg_color);
