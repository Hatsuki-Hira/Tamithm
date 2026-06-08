#pragma once


void update_welcome_ui(void);
void handle_welcome_input(void);


void update_song_select_bar(void);
void init_song_select_windows(void);  // 初始化选歌界面窗口布局
void handle_song_select_input(void);
// 储存每个窗口的对角坐标
extern int song_select_windows_menu[2][2];
extern int song_select_windows_charts[2][2];
extern int song_select_windows_preview[2][2];
extern int song_select_windows_details[2][2];
