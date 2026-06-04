#include <stdio.h>
#include <time.h>
#include <math.h>

#include "../global.h"



// 设置单个字符
void screen_set_cell(
    int y,  // y行
    int x,  // x列
    char ch,  // 字符
    int color  // 底色
)
{
    // 边界检查：防止调整大小时越界写入导致崩溃
    if (y < 0 || y >= user_config.height ||
        x < 0 || x >= user_config.width)
        return;
    if (screen.buffer == NULL || screen.buffer[y] == NULL)
        return;

    Cell *cell = &screen.buffer[y][x];

    cell->ch = ch;
    cell->color = color;
}


// 画平行x轴的线
void screen_draw_line_y(
    int y,
    int color  // 底色
)
{
    for (int x = 0; x < user_config.width; x++)
    {
        // screen.buffer = "\033[47m \033[0m";
        screen_set_cell(y, x, ' ', color);
    }
}


// 画平行y轴的线
void screen_draw_line_x(
    int x,
    int color  // 底色
)
{
    for (int y = 0; y < user_config.height; y++)
    {
        // screen.buffer = "\033[47m \033[0m";
        screen_set_cell(y, x, ' ', color);
    }
}


// 渲染
#define RENDER_BUFFER_SIZE 500000
static char render_buffer[RENDER_BUFFER_SIZE];

// 如果渲染缓冲区快满了，中途 flush 到终端防止溢出
#define RENDER_FLUSH_THRESHOLD (RENDER_BUFFER_SIZE - 4096)
static void render_flush_if_needed(char **pptr) {
    if ((size_t)(*pptr - render_buffer) >= RENDER_FLUSH_THRESHOLD) {
        fwrite(render_buffer, 1, *pptr - render_buffer, stdout);
        *pptr = render_buffer;
    }
}

void render(int frame_waiting) {
//    for(int y = 0; y < user_config.height; y++)
//    {
//        fwrite(screen.buffer[y], 1, user_config.width, stdout);
//        if (y < user_config.height - 1)
//        {
//            putchar('\n');
//        }
//    }

    // 渲染前检测窗口尺寸变化，确保 buffer 与尺寸一致
    screen_resize_detect();

    char *ptr = render_buffer;

    render_flush_if_needed(&ptr);
    ptr += sprintf(ptr, "\033[H");

    for(int y = 0; y < user_config.height; y++)
    {
        int current_color = -1;
        
        for(int x = 0; x < user_config.width; x++)
        {
            render_flush_if_needed(&ptr);

            Cell *cell = &screen.buffer[y][x];
            
            // 如果颜色改变，先输出之前的字符
            if(cell->color != current_color)
            {
                if(current_color != -1)
                {
                    ptr += sprintf(ptr, "\033[0m");
                }
                current_color = cell->color;
                ptr += sprintf(ptr, "\033[%dm", current_color);
            }
            
            *ptr++ = cell->ch;
        }

        // 每行结束恢复默认颜色并换行
        render_flush_if_needed(&ptr);
        ptr += sprintf(ptr, "\033[0m\n");
    }

    fwrite(
        render_buffer,
        1,
        ptr - render_buffer,
        stdout
    );

    fflush(stdout); // 刷新标准输出缓冲区，让画面同时呈现
    if(frame_waiting == 1)
        SLEEP_MS(1000 / user_config.fps);   // 控制帧率
}




// 缓动函数 - 用于非线性动画
// 参数 t: 0~1 的进度值
// 返回: 0~1 的缓动后的进度值

// 线性（无缓动）
float easing_linear(float t) {
    return t;
}

// Ease-in: 开始慢，后来快
float easing_in_quad(float t) {
    return t * t;
}

// Ease-out: 开始快，后来慢
float easing_out_quad(float t) {
    return 1.0f - (1.0f - t) * (1.0f - t);
}

// Ease-in-out: 开始和结束都慢，中间快
float easing_in_out_quad(float t) {
    if (t < 0.5f) {
        return 2.0f * t * t;
    } else {
        return 1.0f - 2.0f * (1.0f - t) * (1.0f - t);
    }
}

// Ease-in-cubic
float easing_in_cubic(float t) {
    return t * t * t;
}

// Ease-out-cubic
float easing_out_cubic(float t) {
    float x = 1.0f - t;
    return 1.0f - x * x * x;
}

// 缓动动画函数指针类型
typedef float (*EasingFunc)(float);



// 带缓动效果的 X 轴扫描动画
void animation_scan_x_easing(
    int start_x_percent,
    int end_x_percent,
    int color,
    int speed_s,
    EasingFunc easing
)
{
    // 初始化值 - 使用浮点除法避免精度丧失
    int step = (start_x_percent < end_x_percent) ? 1 : -1;
    int start_x = (int)(user_config.width * start_x_percent / 100.0);
    int end_x = (int)(user_config.width * end_x_percent / 100.0);

    int total_steps = abs(end_x - start_x);
    if (total_steps == 0) return;
    
    int total_time_ms = speed_s * 1000;
    int last_x = start_x;

    for(int i = 0; i <= total_steps; i++)
    {
        // 计算进度 0~1
        float progress = (float)i / total_steps;
        // 应用缓动函数
        float eased_progress = easing(progress);
        // 计算当前位置
        int current_x = start_x + (int)(step * total_steps * eased_progress);
        
        // 只绘制从上一个位置到当前位置之间的新增列
        if (step > 0) {
            for (int x = last_x; x <= current_x; x++) {
                screen_draw_line_x(x, color);
            }
        } else {
            for (int x = last_x; x >= current_x; x--) {
                screen_draw_line_x(x, color);
            }
        }
        last_x = current_x;
        
        render(0);

        // 计算延迟时间
        int step_time = (i < total_steps) ? (total_time_ms / total_steps) : 0;
        SLEEP_MS(step_time);
    }
}

// 带缓动效果的 Y 轴扫描动画
void animation_scan_y_easing(
    int start_y_percent,
    int end_y_percent,
    int color,
    int speed_s,
    EasingFunc easing
)
{
    // 初始化值
    int step = (start_y_percent < end_y_percent) ? 1 : -1;
    int start_y = (int)(user_config.height * start_y_percent / 100.0);
    int end_y = (int)(user_config.height * end_y_percent / 100.0);

    int total_steps = abs(end_y - start_y);
    if (total_steps == 0) return;
    
    int total_time_ms = speed_s * 1000;
    int last_y = start_y;

    for(int i = 0; i <= total_steps; i++)
    {
        // 计算进度 0~1
        float progress = (float)i / total_steps;
        // 应用缓动函数
        float eased_progress = easing(progress);
        // 计算当前位置
        int current_y = start_y + (int)(step * total_steps * eased_progress);
        
        // 只绘制从上一个位置到当前位置之间的新增行
        if (step > 0) {
            for (int y = last_y; y <= current_y; y++) {
                screen_draw_line_y(y, color);
            }
        } else {
            for (int y = last_y; y >= current_y; y--) {
                screen_draw_line_y(y, color);
            }
        }
        last_y = current_y;
        
        render(0);

        // 计算延迟时间
        int step_time = (i < total_steps) ? (total_time_ms / total_steps) : 0;
        SLEEP_MS(step_time);
    }
}


// 直线扫描填充动画(使用屏幕百分比定位)
void animation_scan_y(
    int start_y_percent,
    int end_y_percent,
    int color,
    int speed_s
)
{
    // 使用线性缓动（原来的行为）
    animation_scan_y_easing(start_y_percent, end_y_percent, color, speed_s, easing_linear);
}

void animation_scan_x(
    int start_x_percent,
    int end_x_percent,
    int color,
    int speed_s
)
{
    // 使用线性缓动（原来的行为）
    animation_scan_x_easing(start_x_percent, end_x_percent, color, speed_s, easing_linear);
}