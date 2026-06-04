#pragma once

// 加载图像到屏幕缓冲区
// image_path: 图像文件路径
// start_x, start_y: 在屏幕上的起始位置
// max_width, max_height: 最大显示尺寸（0表示使用实际尺寸）
void load_image_to_screen(const char *image_path, int start_x, int start_y, int max_width, int max_height);

