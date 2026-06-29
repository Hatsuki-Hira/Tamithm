#include <stdio.h>
#include "../global.h"

// 音频库
#include "miniaudio/stb_vorbis.c"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"


ma_engine engine;
ma_sound song;       // 歌曲
ma_sound hit_sound;  // 打击音效
ma_result result;
int audio_init(void)
{
    // 音频引擎初始化
    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        return -1;
    }

    // 预加载打击音效
    if (ma_sound_init_from_file(&engine, "sounds/soft-hitNormal.wav", 0, NULL, NULL, &hit_sound) == MA_SUCCESS) {
        ma_sound_set_volume(&hit_sound, 0.6f);
    }
    return 0;
}

// 释放歌曲
static void audio_uninit_song(void) {
    ma_sound_stop(&song);
    ma_sound_uninit(&song);
}

int audio_play_song(void)
{
    char filepath[512];

    // 尝试 .mp3（流式播放，不整首解码到内存）
    sprintf(filepath, "%s%s.mp3", chart_full_path, chart_names[selected_chart_num]);
    result = ma_sound_init_from_file(&engine, filepath, MA_SOUND_FLAG_STREAM, NULL, NULL, &song);
    if (result == MA_SUCCESS) {
        // 如果 music_offset > 0，则延迟指定毫秒后开始播放
        if (user_config.music_offset >= 0) {
            ma_uint64 now = ma_engine_get_time_in_milliseconds(&engine);
            ma_sound_set_start_time_in_milliseconds(&song, now + user_config.music_offset + START_RELAY_MS);  // 游玩前的空白准备时间
        }
        ma_sound_start(&song);
        return 0;
    }

    // 尝试 .ogg（流式播放）
    sprintf(filepath, "%s%s.ogg", chart_full_path, chart_names[selected_chart_num]);
    result = ma_sound_init_from_file(&engine, filepath, MA_SOUND_FLAG_STREAM, NULL, NULL, &song);
    if (result == MA_SUCCESS) {
        if (user_config.music_offset >= 0) {
            ma_uint64 now = ma_engine_get_time_in_milliseconds(&engine);
            ma_sound_set_start_time_in_milliseconds(&song, now + user_config.music_offset + START_RELAY_MS);
        }
        ma_sound_start(&song);
        return 0;
    }

    return -1;
}

void audio_exit(void)
{
    audio_uninit_song();

    ma_sound_stop(&hit_sound);
    ma_sound_uninit(&hit_sound);

    ma_engine_uninit(&engine);
}

void tap_sound_effect(void)
{
    ma_sound_seek_to_pcm_frame(&hit_sound, 0);  // 回弹到开头
    ma_sound_start(&hit_sound);
}