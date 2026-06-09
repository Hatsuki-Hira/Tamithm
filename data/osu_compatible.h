#pragma once

#include "../global.h"



// 从 .osu 文件中读取所有 Note
// 返回 Note 数组和数量，需要外部提供存储位置
// 成功返回 0，失败返回 -1
int osu_load_notes(
    const char *filepath,   // .osu 文件路径
    Note **out_notes,       // 输出 Note 数组（malloc 分配，调用者 free）
    int *out_count          // 输出 Note 数量
);

// 将 x 坐标映射到轨道号（0 ~ keys-1）
int osu_x_to_lane(int x, int keys);
