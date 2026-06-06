#include <stdio.h>

#include "../global.h"
#include "../data/renderer.h"



// 
void update_song_select_ui0(void) {
    display_text(user_config.width * 0.5 - 3, user_config.height * 0.5, "Tamithm", 107);
    render(0);
    SLEEP_MS(1000);
}