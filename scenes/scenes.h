#pragma once


void update_welcome_ui(void);
void handle_welcome_input(void);



// 初始化选歌界面窗口布局
void init_song_select_windows(void);
void update_song_select_ui(void);
void handle_song_select_input(void);



// 初始化打歌界面窗口布局
void init_playing_windows(void);
void reset_playing_state(void);
void update_playing_ui(void);
void update_note(void);
void handle_playing_input(void);


// 结算界面
void score_caculate(void);
void update_result_ui(void);
void handle_result_input(void);
