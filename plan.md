# Terminal Rhythm Game Development Plan

## 项目目标

开发一个基于 C 语言终端（TUI）的下落式音游——Tamithm，实现：

* 动态终端渲染
* 场景切换系统
* osu! 谱面读取
* 实时音符下落
* 打击判定
* 计分与成绩结算

最终形成一个完整可游玩的终端音游框架。

---

# Milestone 1：底层渲染框架

## 目标

构建终端游戏运行环境。

## 内容

### 简易屏幕缓冲区

实现单缓冲渲染：

```text
 Buffer
    ↓
Terminal
```

功能：

* 字符绘制
* 清屏与光标控制

### 完整屏幕缓冲区

完整实现单缓冲渲染：

```text
 Buffer
    ↓
Terminal
```

功能：

* 字符串绘制
* ANSI颜色支持(前景/背景色)
* 局部刷新

### 终端适配

实现：

* 获取终端尺寸
* 动态窗口缩放
* 帧率控制
* 定时器系统

### 测试

完成：

* 指定位置输出字符
* 彩色字符输出
* 简单画面渲染

---

# Milestone 2：输入系统

## 目标

实现统一键盘输入接口。

## 内容

封装：

```text
Up
Down
Left
Right
Enter
Esc
Space
```

实现：

* 非阻塞输入
* 长按检测
* 单次按下检测

### 测试

实现菜单上下移动。

---

# Milestone 3：场景系统

## 目标

实现游戏状态机。

## 内容

设计 Scene 接口：

```text
Welcome
Song_Select
Setting
Playing
Result
```

实现场景管理器。

### Scene 规划

#### Title Scene

游戏标题界面

功能：

* Logo显示
* 开始游戏

#### Select Scene

选曲与设置界面

功能：

* 歌曲列表
* 谱面难度
* 设置菜单

#### Gameplay Scene

游戏游玩界面

功能：

* 音符渲染
* 判定线显示

#### Result Scene

成绩结算界面

功能：

* 分数统计
* Accuracy
* Combo
* Rank

---

# Milestone 4：窗口系统（UI Framework）

## 目标

构建可复用 TUI 组件。

## 内容

### Layout

动态布局系统：

```text
Rect
 ├ x
 ├ y
 ├ w
 └ h
```

### Widget

实现：

* 窗口边框
* 窗口标签

### 应用

用于：

* 选曲界面
* 设置界面
* 成绩界面

---

# Milestone 5：谱面系统

## 目标

实现谱面读取与管理。

## 内容

### osu! 文件解析

读取：

* Metadata
* TimingPoints
* HitObjects

支持：

```text
.osu
```

### Note 数据结构

```text
Time
Lane
Type
```

### Chart Loader

实现：

* 谱面扫描
* 谱面加载
* 谱面缓存

### 测试

打印谱面统计信息。

---

# Milestone 6：音符播放系统

## 目标

实现谱面可视化。

## 内容

### 时间轴

建立：

```text
Song Time
↓
Note Time
↓
Position
```

### Note Renderer

实现：

* 音符生成
* 音符移动
* 音符销毁

### 测试

确认谱面与时间轴同步。

---

# Milestone 7：判定系统

## 目标

实现玩家游玩功能。

## 内容

### 输入判定

根据：

```text
当前时间
与
音符时间
```

计算：

* Perfect
* Great
* Good
* Miss

### Combo 系统

实现：

* 连击数
* Combo Break

### Accuracy 系统

实时计算：

* Accuracy

---

# Milestone 8：计分系统

## 目标

实现完整成绩计算。

## 内容

统计：

* Score
* Accuracy
* Max Combo
* 判定数量

实现评级：

```text
SS
S
A
B
C
D
```

---

# Milestone 9：结算界面

## 目标

完成完整游戏流程。

## 内容

显示：

* 分数
* Accuracy
* Combo
* Rank

支持：

* 返回选曲
* 重新开始
* 退出游戏

---

# Milestone 10：优化与扩展

## 内容

性能优化：

* 局部刷新
* 减少终端输出

视觉优化：

* 颜色主题
* 动画效果

功能扩展：

* 自动演奏（Autoplay）
* 回放系统
* 谱面编辑器
* 本地成绩保存

---

# 最终流程

```text
启动
 ↓
Title
 ↓
Song Select
 ↓
Gameplay
 ↓
Result
 ↓
Song Select
```

完成一个完整的终端音游闭环。
