#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

// 定义终端尺寸（可以根据需要调整，或者动态获取）
#define WIDTH 80
#define HEIGHT 24

// 定义点的状态
typedef enum {
    STATE_HASH,   // 全部是 #
    STATE_CODE,   // 乱跳的代码
    STATE_SPACE   // 变成空格
} PointState;

typedef struct {
    char current_char;
    PointState state;
    int code_timer; // 控制乱跳代码持续的帧数
} Cell;

// 清屏并隐藏光标
void init_terminal() {
    printf("\033[2J");    // 清屏
    printf("\033[?25l");   // 隐藏光标
    fflush(stdout);
}

// 恢复光标显示
void reset_terminal() {
    printf("\033[?25h");   // 显示光标
    printf("\033[%d;1H\n", HEIGHT + 1); // 移动到屏幕下方
    fflush(stdout);
}

// 精准移动光标到 (x, y) 坐标 (注意：ANSI从1开始计数)
void move_cursor(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1);
}

// 随机生成一个“乱跳代码”字符
char get_random_code_char() {
    // 选用一些看起来像代码或机器码的可见字符
    const char code_pool[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()_+-=";
    int size = sizeof(code_pool) - 1;
    return code_pool[rand() % size];
}

int main() {
    srand((unsigned int)time(NULL));
    init_terminal();

    Cell screen[HEIGHT][WIDTH];
    int active_cells = WIDTH * HEIGHT;

    // 初始化：全屏幕满载 '#'
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            screen[y][x].current_char = '#';
            screen[y][x].state = STATE_HASH;
            // 每个点乱跳的持续帧数随机，这样才会有交错渐变的效果
            screen[y][x].code_timer = 10 + rand() % 20; 
            
            // 初始打印满屏 #
            move_cursor(x, y);
            putchar('#');
        }
    }
    fflush(stdout);
    SLEEP_MS(500); // 满屏 # 稍微停留一下下

    // 主动画循环
    while (active_cells > 0) {
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                // 如果已经是空格，说明该点动画结束，跳过
                if (screen[y][x].state == STATE_SPACE) {
                    continue;
                }

                // 状态机转换与字符更新
                if (screen[y][x].state == STATE_HASH) {
                    // 每一帧有一定概率让 # 塌陷成乱跳的代码
                    if (rand() % 100 < 8) { 
                        screen[y][x].state = STATE_CODE;
                        screen[y][x].current_char = get_random_code_char();
                        move_cursor(x, y);
                        putchar(screen[y][x].current_char);
                    }
                } 
                else if (screen[y][x].state == STATE_CODE) {
                    screen[y][x].code_timer--;
                    
                    if (screen[y][x].code_timer <= 0) {
                        // 计时结束，变成空格
                        screen[y][x].state = STATE_SPACE;
                        screen[y][x].current_char = ' ';
                        active_cells--; // 剩余活跃点减少
                    } else {
                        // 还在乱跳期间，持续变换字符
                        screen[y][x].current_char = get_random_code_char();
                    }
                    
                    // 局部刷新
                    move_cursor(x, y);
                    putchar(screen[y][x].current_char);
                }
            }
        }
        
        fflush(stdout); // 刷新标准输出缓冲区，让画面同时呈现
        SLEEP_MS(30);   // 控制帧率，大约每秒 30 帧
    }

    reset_terminal();
    return 0;
}