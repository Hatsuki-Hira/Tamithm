#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <stdlib.h>

#include "../global.h"
#include "../data/renderer.h"

void load_image_to_screen(const char *image_path, int start_x, int start_y, int max_width, int max_height) {
    int width, height, channels;
    unsigned char *image = stbi_load(image_path, &width, &height, &channels, 0);
    if (!image) {
        printf("Failed to load image: %s\n", image_path);
        return;
    }

    // 计算缩放尺寸（考虑最大宽高限制）
    int display_width = width;
    int display_height = height / 2;  // 因为用了半块字符，高度折半
    
    if (max_width > 0 && display_width > max_width) {
        float scale = (float)max_width / display_width;
        display_width = max_width;
        display_height = (int)(display_height * scale);
    }
    
    if (max_height > 0 && display_height > max_height) {
        float scale = (float)max_height / display_height;
        display_height = max_height;
        display_width = (int)(display_width * scale);
    }

    // 确保不超出屏幕边界
    if (start_x + display_width > user_config.width) {
        display_width = user_config.width - start_x;
    }
    if (start_y + display_height > user_config.height) {
        display_height = user_config.height - start_y;
    }

    // 计算缩放系数
    float x_scale = (float)width / display_width;
    float y_scale = (float)height / (display_height * 2);

    // 转换图像到屏幕缓冲区
    for (int y = 0; y < display_height; y++) {
        for (int x = 0; x < display_width; x++) {
            // 计算源图像的坐标
            int src_x = (int)(x * x_scale);
            int src_y = (int)(y * y_scale * 2);
            
            // 边界检查
            if (src_x >= width) src_x = width - 1;
            if (src_y + 1 >= height) src_y = height - 2;
            if (src_y < 0) src_y = 0;

            unsigned char *pixel_top = image + (src_y * width + src_x) * channels;
            unsigned char *pixel_bottom = image + ((src_y + 1) * width + src_x) * channels;

            int r1 = pixel_top[0], g1 = pixel_top[1], b1 = pixel_top[2];
            int r2 = pixel_bottom[0], g2 = pixel_bottom[1], b2 = pixel_bottom[2];
            int a1 = (channels == 4) ? pixel_top[3] : 255;
            int a2 = (channels == 4) ? pixel_bottom[3] : 255;

            int screen_y = start_y + y;
            int screen_x = start_x + x;

            // 检查屏幕边界
            if (screen_x < 0 || screen_x >= user_config.width || 
                screen_y < 0 || screen_y >= user_config.height) {
                continue;
            }

            // 根据透明度选择显示的字符
            // 使用彩色空格来表示像素，转换 RGB 到 ANSI 颜色
            // 简化方案：用空格 + 背景色表示
            if (channels == 4) {
                if (a1 == 0 && a2 == 0) {
                    // 都透明 - 黑色背景
                    screen_set_cell(screen_y, screen_x, ' ', 40);
                } else if (a1 != 0 && a2 == 0) {
                    // 只上层可见 - 使用上层颜色
                    // 简化：用灰度转换到 ANSI 颜色 30-37
                    int gray = (r1 + g1 + b1) / 3;
                    int color_code = 40 + (gray / 36);  // 0-7 的颜色范围
                    screen_set_cell(screen_y, screen_x, ' ', color_code);
                } else if (a1 == 0 && a2 != 0) {
                    // 只下层可见
                    int gray = (r2 + g2 + b2) / 3;
                    int color_code = 40 + (gray / 36);
                    screen_set_cell(screen_y, screen_x, ' ', color_code);
                } else {
                    // 都可见 - 混合颜色
                    int gray = ((r1 + r2) / 2 + (g1 + g2) / 2 + (b1 + b2) / 2) / 3;
                    int color_code = 40 + (gray / 36);
                    screen_set_cell(screen_y, screen_x, ' ', color_code);
                }
            } else {
                // RGB 无透明度 - 直接转换颜色
                int gray = (r1 + g1 + b1) / 3;
                int color_code = 40 + (gray / 36);
                screen_set_cell(screen_y, screen_x, ' ', color_code);
            }
        }
    }

    stbi_image_free(image);
}
