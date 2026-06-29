# Tamithm

这是一个由纯c编写的终端下落式音乐游戏，目前只有4K模式可以游玩

## 使用
终端运行run.exe即可，进入游戏标题页面 **按下任意按键** 进入游戏，按esc可以返回上一页面，在标题页面按esc可退出

## 谱面兼容性
| 音乐游戏 | 状态 | 说明 |
|------|------|------|
| Osu! | ✅ | mania模式4k谱面 |

## 功能实现
| 功能 | 状态 | 说明 |
|------|------|------|
| OSU!谱面难度算法还原 | ✅ | 由Gemini生成... |
| 过场动画 | ✅ | 对全角字符(中日文字符)会有抖动问题 |
| 游玩结算 | ✅ | 游玩成绩暂不可保存 |
| 多种音效 | ➡️ | 打击音效已完成 |
| 难度星级颜色提示 | ➡️ | 正在实现 |
| 延迟调整 | ➡️ | 正在实现 |
| 上隐 | ➡️ | 正在实现 |
| Autoplay | ❌ | 咕咕咕 |
| 主题功能 | ❌ | 咕咕咕 |

## 编译
项目主目录执行
```
cd .\Tamithm
gcc -g .\*.c .\scenes\*.c .\data\*.c -o .\run.exe
```

## 文件结构
```
Tamithm/
├── data/   # 主程序
│   ├── miniaudio/         # 音频库头文件
│   │   └── ...
│   ├── renderer.c         # 终端TUI渲染
│   ├── audio_system.c     # 音频系统
│   ├── osu_compatible.c   # 游戏osu!谱面文件兼容
│   ├── rating_caculate.c  # 谱面难度算法
│   └── ...
├── scenes/   # 显示界面(场景)划分
│   ├── scenes.h         # 入口头文件
│   ├── welcome.c        # 欢迎界面
│   ├── song_select.c    # 关卡选择菜单/设置菜单
│   ├── playing.c        # 游玩界面
│   ├── result.c         # 结算界面
│   └── ...
├── charts/   # 谱面存放位置
│   └── ...
├── sounds/   # 音效
│   └── ...
├── run.c      # 程序入口
├── global.h   # 各种全局参数
├── userdata   # 用户数据
└── README.md
```

## 使用项目
- miniaudio 音频库
  - https://github.com/mackron/miniaudio