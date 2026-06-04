#include <stdio.h>
#include <string.h>

#include "../global.h"
#include "renderer.h"

const char ch_a[4][5] = {
    "0000",
    "0110",
    "1010",
    "0110"
};

const char ch_b[4][5] = {
    "1000",
    "1100",
    "1010",
    "1100"
};

const char ch_c[4][5] = {
    "0000",
    "0110",
    "1000",
    "0110"
};

const char ch_d[4][5] = {
    "0010",
    "0110",
    "1010",
    "0110"
};

const char ch_e[4][5] = {
    "0110",
    "0110",
    "1000",
    "0110"
};

const char ch_f[4][5] = {
    "0110",
    "0100",
    "1110",
    "0100"
};

const char ch_g[4][5] = {
    "0110",
    "1110",
    "0010",
    "1100"
};

const char ch_h[4][5] = {
    "1000",
    "1100",
    "1010",
    "1010"
};

const char ch_i[4][5] = {
    "0100",
    "0000",
    "0100",
    "0100"
};

const char ch_j[4][5] = {
    "0010",
    "0010",
    "0010",
    "1100"
};

const char ch_k[4][5] = {
    "1000",
    "1010",
    "1100",
    "1010"
};

const char ch_l[4][5] = {
    "1000",
    "1000",
    "1000",
    "1100"
};

const char ch_m[4][7] = {
    "000000",
    "110100",
    "101010",
    "101010"
};

const char ch_n[4][5] = {
    "0000",
    "1100",
    "1010",
    "1010"
};

const char ch_o[4][5] = {
    "0000",
    "1110",
    "1010",
    "1110"
};

const char ch_p[4][5] = {
    "1100",
    "1010",
    "1100",
    "1000"
};

const char ch_q[4][5] = {
    "0110",
    "1010",
    "0110",
    "0010"
};

const char ch_r[4][5] = {
    "0000",
    "1110",
    "1000",
    "1000"
};

const char ch_s[4][5] = {
    "0110",
    "1100",
    "0110",
    "1100"
};

const char ch_t[4][5] = {
    "0100",
    "1110",
    "0100",
    "0110"
};

const char ch_u[4][5] = {
    "0000",
    "1010",
    "1010",
    "0110"
};

const char ch_v[4][5] = {
    "0000",
    "1010",
    "1010",
    "0100"
};

const char ch_w[4][6] = {
    "00000",
    "10101",
    "10101",
    "01010"
};

const char ch_x[4][5] = {
    "0000",
    "1010",
    "0100",
    "1010"
};

const char ch_y[4][5] = {
    "0000",
    "1010",
    "0100",
    "0100"
};


const char ch_z[4][5] = {
    "1110",
    "0010",
    "0100",
    "1110"
};


// 大写字母
const char ch_A[4][5] = {
    "0100",
    "1010",
    "1110",
    "1010"
};

const char ch_B[4][5] = {
    "1100",
    "1110",
    "1010",
    "1110"
};

const char ch_C[4][5] = {
    "0110",
    "1000",
    "1000",
    "0110"
};

const char ch_D[4][5] = {
    "1100",
    "1010",
    "1010",
    "1100"
};

const char ch_E[4][5] = {
    "1110",
    "1000",
    "1100",
    "1110"
};

const char ch_F[4][5] = {
    "1110",
    "1000",
    "1100",
    "1000"
};

const char ch_G[4][5] = {
    "0110",
    "1000",
    "1010",
    "0110"
};

const char ch_H[4][5] = {
    "1010",
    "1110",
    "1010",
    "1010"
};

const char ch_I[4][5] = {
    "1110",
    "0100",
    "0100",
    "1110"
};

const char ch_J[4][5] = {
    "0110",
    "0010",
    "0010",
    "1100"
};

const char ch_K[4][5] = {
    "1010",
    "1100",
    "1010",
    "1010"
};

const char ch_L[4][5] = {
    "1000",
    "1000",
    "1000",
    "1110"
};

const char ch_M[4][7] = {
    "110011",
    "101101",
    "101101",
    "100001"
};

const char ch_N[4][5] = {
    "0000",
    "1110",
    "1010",
    "1010"
};

const char ch_O[4][5] = {
    "0100",
    "1010",
    "1010",
    "0100"
};

const char ch_P[4][5] = {
    "1110",
    "1010",
    "1110",
    "1000"
};

const char ch_Q[4][5] = {
    "0110",
    "1010",
    "1010",
    "0111"
};

const char ch_R[4][5] = {
    "1110",
    "1010",
    "1100",
    "1010"
};

const char ch_S[4][5] = {
    "0110",
    "1000",
    "0110",
    "1100"
};

const char ch_T[4][5] = {
    "1110",
    "0100",
    "0100",
    "0100"
};

const char ch_U[4][5] = {
    "1010",
    "1010",
    "1010",
    "0110"
};

const char ch_V[4][5] = {
    "1010",
    "1010",
    "1010",
    "0100"
};

const char ch_W[4][6] = {
    "10101",
    "10101",
    "10101",
    "01010"
};

const char ch_X[4][5] = {
    "1010",
    "0100",
    "0100",
    "1010"
};

const char ch_Y[4][5] = {
    "1010",
    "0100",
    "0100",
    "0100"
};

const char ch_Z[4][5] = {
    "1110",
    "0010",
    "0100",
    "1110"
};

// 显示单个大字体字符
void display_char(int x, int y, char ch, int color) {
    const void *pattern = NULL;
    int width = 0;
    
    // 获取对应字符的点阵
    switch(ch) {
        // 小写字母
        case 'a': pattern = (const void *)ch_a; width = 4; break;
        case 'b': pattern = (const void *)ch_b; width = 4; break;
        case 'c': pattern = (const void *)ch_c; width = 4; break;
        case 'd': pattern = (const void *)ch_d; width = 4; break;
        case 'e': pattern = (const void *)ch_e; width = 4; break;
        case 'f': pattern = (const void *)ch_f; width = 4; break;
        case 'g': pattern = (const void *)ch_g; width = 4; break;
        case 'h': pattern = (const void *)ch_h; width = 4; break;
        case 'i': pattern = (const void *)ch_i; width = 4; break;
        case 'j': pattern = (const void *)ch_j; width = 4; break;
        case 'k': pattern = (const void *)ch_k; width = 4; break;
        case 'l': pattern = (const void *)ch_l; width = 4; break;
        case 'm': pattern = (const void *)ch_m; width = 6; break;
        case 'n': pattern = (const void *)ch_n; width = 4; break;
        case 'o': pattern = (const void *)ch_o; width = 4; break;
        case 'p': pattern = (const void *)ch_p; width = 4; break;
        case 'q': pattern = (const void *)ch_q; width = 4; break;
        case 'r': pattern = (const void *)ch_r; width = 4; break;
        case 's': pattern = (const void *)ch_s; width = 4; break;
        case 't': pattern = (const void *)ch_t; width = 4; break;
        case 'u': pattern = (const void *)ch_u; width = 4; break;
        case 'v': pattern = (const void *)ch_v; width = 4; break;
        case 'w': pattern = (const void *)ch_w; width = 5; break;
        case 'x': pattern = (const void *)ch_x; width = 4; break;
        case 'y': pattern = (const void *)ch_y; width = 4; break;
        case 'z': pattern = (const void *)ch_z; width = 4; break;
        
        // 大写字母
        case 'A': pattern = (const void *)ch_A; width = 4; break;
        case 'B': pattern = (const void *)ch_B; width = 4; break;
        case 'C': pattern = (const void *)ch_C; width = 4; break;
        case 'D': pattern = (const void *)ch_D; width = 4; break;
        case 'E': pattern = (const void *)ch_E; width = 4; break;
        case 'F': pattern = (const void *)ch_F; width = 4; break;
        case 'G': pattern = (const void *)ch_G; width = 4; break;
        case 'H': pattern = (const void *)ch_H; width = 4; break;
        case 'I': pattern = (const void *)ch_I; width = 4; break;
        case 'J': pattern = (const void *)ch_J; width = 4; break;
        case 'K': pattern = (const void *)ch_K; width = 4; break;
        case 'L': pattern = (const void *)ch_L; width = 4; break;
        case 'M': pattern = (const void *)ch_M; width = 6; break;
        case 'N': pattern = (const void *)ch_N; width = 4; break;
        case 'O': pattern = (const void *)ch_O; width = 4; break;
        case 'P': pattern = (const void *)ch_P; width = 4; break;
        case 'Q': pattern = (const void *)ch_Q; width = 4; break;
        case 'R': pattern = (const void *)ch_R; width = 4; break;
        case 'S': pattern = (const void *)ch_S; width = 4; break;
        case 'T': pattern = (const void *)ch_T; width = 4; break;
        case 'U': pattern = (const void *)ch_U; width = 4; break;
        case 'V': pattern = (const void *)ch_V; width = 4; break;
        case 'W': pattern = (const void *)ch_W; width = 5; break;
        case 'X': pattern = (const void *)ch_X; width = 4; break;
        case 'Y': pattern = (const void *)ch_Y; width = 4; break;
        case 'Z': pattern = (const void *)ch_Z; width = 4; break;
        default: return;
    }
    
    if (!pattern) return;
    
    // 转换为字符指针来访问数据
    const char *data = (const char *)pattern;
    
    // 绘制字符的点阵
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < width; col++) {
            // 计算在行中的位置（考虑行步长）
            char pixel = data[row * (width + 1) + col];  // width+1 是因为有null终止符
            if (pixel == '1') {
                screen_set_cell(y + row, x + col, ' ', color);
            }
        }
    }
}

// 显示字符串
void display_text(int x, int y, const char *text, int color) {
    int current_x = x;
    
    for (int i = 0; text[i] != '\0'; i++) {
        display_char(current_x, y, text[i], color);
        
        // 计算下一个字符的 x 位置（加上间距）
        int width = 4;
        if (text[i] == 'm' || text[i] == 'M') width = 6;
        else if (text[i] == 'w' || text[i] == 'W') width = 5;
        
        current_x += width + 1;  // 字符宽度 + 1 像素间距
    }
}