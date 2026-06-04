#pragma once

// 显示单个大字体字符
// 参数：x, y - 左上角位置，ch - 字符，color - 背景色
void display_char(int x, int y, char ch, int color);

// 显示字符串
// 参数：x, y - 左上角位置，text - 字符串，color - 背景色
void display_text(int x, int y, const char *text, int color);
