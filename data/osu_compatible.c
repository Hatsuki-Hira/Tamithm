#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../global.h"
#include "osu_compatible.h"





// 将 x 坐标映射到轨道号
// osu!mania 标准：4K 时 x = 64/192/320/448 → 0/1/2/3
// 更通用的算法：将 x 范围 [64, 448] 等分成 keys 份
int osu_x_to_lane(int x, int keys) {
    // 每根轨道的宽度
    int lane_width = (448 - 64) / keys;
    int lane = (x - 64) / lane_width;
    if (lane < 0) lane = 0;
    if (lane >= keys) lane = keys - 1;
    return lane;
}



// 从 extra 字段中提取 Hold 结束时间
// extra 格式: "3000:0:0:0:50:finish.wav" → 返回 3000
static int parse_hold_endtime(const char *extra) {
    if (extra == NULL || extra[0] == '\0')
        return -1;

    // 结束时间在第一个冒号之前
    const char *colon = strchr(extra, ':');
    if (colon == NULL)
        return -1;

    // 提取冒号前的数字部分
    int len = colon - extra;
    char buf[32];
    if (len >= (int)sizeof(buf))
        len = sizeof(buf) - 1;
    memcpy(buf, extra, len);
    buf[len] = '\0';

    return atoi(buf);
}



int all_note_count = 0;  // 初始化包括尾判的物量
#define INITIAL_NOTE_CAPACITY 1024
#define NOTE_GROW_FACTOR 2

int osu_load_notes(
    const char *filepath,
    Note **out_notes,
    int *out_count
) {
    FILE *fp = fopen(filepath, "r");
    if (fp == NULL)
        return -1;

    int capacity = INITIAL_NOTE_CAPACITY;
    int count = 0;  // 纯头部物量
    int temp_all_note_count = 0;  // 包括尾判的物量
    Note *notes = malloc(capacity * sizeof(Note));
    if (notes == NULL) {
        fclose(fp);
        return -1;
    }

    // 先读取 [Difficulty] 下的 CircleSize 确定轨道数
    int keys = 4;  // 默认 4K
    char line[512];
    int in_hit_objects = 0;

    while (fgets(line, sizeof(line), fp)) {
        // 去掉末尾换行符
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (len > 1 && line[len - 2] == '\r') line[len - 2] = '\0';

        // 找到 [HitObjects] 段
        if (strcmp(line, "[HitObjects]") == 0) {
            in_hit_objects = 1;
            continue;
        }

        // 如果在 [HitObjects] 之前，读取 CircleSize
        if (!in_hit_objects) {
            if (strncmp(line, "CircleSize:", 11) == 0) {
                keys = atoi(line + 11);
                if (keys < 1) keys = 4;
            }
            continue;
        }

        // 遇到下一个段标题则停止
        if (line[0] == '[') {
            break;
        }

        // 跳过空行
        if (line[0] == '\0')
            continue;

        // -------- 解析 Note 行 --------
        // 格式: x,y,time,type,hitSound,extra
        int x, y, time, type, hitSound;
        char extra[256];

        int parsed = sscanf(line, "%d,%d,%d,%d,%d,%255s",
                            &x, &y, &time, &type, &hitSound, extra);
        if (parsed < 5)
            continue;

        // 扩容
        if (count >= capacity) {
            capacity *= NOTE_GROW_FACTOR;
            Note *new_notes = realloc(notes, capacity * sizeof(Note));
            if (new_notes == NULL) {
                free(notes);
                fclose(fp);
                return -1;
            }
            notes = new_notes;
        }

        Note *note = &notes[count];

        note->lane = osu_x_to_lane(x, keys);
        note->start_time = time + START_RELAY_MS;

        if (type == 128) {
            // Hold Note
            note->type = NOTE_HOLD;
            int end_time = (parsed >= 6) ? parse_hold_endtime(extra) : -1;
            if (end_time < time)
                end_time = time + 500;  // 保底长度(似乎可以去掉了，短Hold没问题)
            note->end_time = end_time + START_RELAY_MS;

            // 尾判算一个物量，补偿一个
            temp_all_note_count++;
        } else {
            // Tap Note（type=1 或其他）
            note->type = NOTE_TAP;
            note->end_time = -1;
        }

        count++;
    }

    fclose(fp);

    *out_notes = notes;
    *out_count = count;
    all_note_count = count + temp_all_note_count;  // 包括尾判的物量
    return 0;
}
