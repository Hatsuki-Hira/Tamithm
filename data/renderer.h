#pragma once



void screen_set_cell(
    int y,
    int x,
    char ch,
    int color
);

void screen_draw_line_y(
    int y,
    int color  // 底色
);

void screen_draw_line_x(
    int x,
    int color  // 底色
);

void render(int frame_waiting);

// 基础动画函数（线性）
void animation_scan_y(
    int start_y_percent,
    int end_y_percent,
    int color,
    int speed_s
);

void animation_scan_x(
    int start_x_percent,
    int end_x_percent,
    int color,
    int speed_s
);

// 缓动函数指针类型
typedef float (*EasingFunc)(float);

// 缓动函数声明
float easing_linear(float t);
float easing_in_quad(float t);
float easing_out_quad(float t);
float easing_in_out_quad(float t);
float easing_in_cubic(float t);
float easing_out_cubic(float t);

// 带缓动效果的动画函数
void animation_scan_y_easing(
    int start_y_percent,
    int end_y_percent,
    int color,
    int speed_s,
    EasingFunc easing
);

void animation_scan_x_easing(
    int start_x_percent,
    int end_x_percent,
    int color,
    int speed_s,
    EasingFunc easing
);