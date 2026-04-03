# C++ 2D 贪吃蛇游戏

这是一个使用C++和SDL2开发的简单2D贪吃蛇游戏。

## 项目结构

```
My2DGame/
├── CMakeLists.txt          # CMake构建配置
├── build_game.bat          # Windows编译脚本
├── src/                    # 源代码目录
│   ├── main.cpp           # 程序入口
│   ├── game.h             # 游戏主类头文件
│   ├── game.cpp           # 游戏主类实现
│   ├── player.h           # 玩家（蛇）头文件
│   ├── player.cpp         # 玩家类实现
│   ├── food.h             # 食物头文件
│   └── food.cpp           # 食物类实现
└── README.md              # 项目说明
```

## 功能特点

- 🎮 完整的贪吃蛇游戏逻辑
- 🐍 蛇身生长机制
- 🍎 随机食物生成和碰撞检测
- 🎯 分数系统
- 🎨 简单但美观的图形界面
- ⌨️ 键盘控制支持

## 控制说明

- **方向键** 或 **WASD** - 控制蛇的移动方向
- **R** - 重新开始游戏
- **ESC** - 退出游戏
- **关闭窗口** - 退出游戏

## 游戏规则

1. 控制绿色的蛇在屏幕中移动
2. 吃到彩色的食物可以增长蛇身
3. 每吃到一个食物获得10分
4. 撞到边界或墙壁游戏结束
5. 重新开始游戏时分数会重置

## 系统要求

- **Windows 10/11** (本项目主要为Windows设计)
- **Visual Studio 2019** 或更新版本
- **CMake 3.15** 或更新版本
- **Internet连接** (用于自动下载SDL2)

## 快速开始

### 方法1：使用批处理脚本（推荐）

1. 双击运行 `build_game.bat`
2. 脚本会自动：
   - 检查必要的工具
   - 创建构建目录
   - 下载和配置SDL2
   - 编译项目
   - 启动游戏

### 方法2：手动编译

```bash
# 1. 创建构建目录
mkdir build
cd build

# 2. 配置CMake
cmake .. -G "Visual Studio 16 2019" -A x64

# 3. 编译项目
cmake --build . --config Release

# 4. 运行游戏
.\Release\MyGame.exe
```

## 技术细节

### 使用的主要技术

- **SDL2** - 跨平台多媒体库，用于窗口管理和图形渲染
- **CMake FetchContent** - 现代CMake功能，自动下载依赖
- **C++17** - 使用现代C++特性
- **面向对象设计** - 清晰的类结构

### 代码结构

- **Game类** - 游戏主循环、事件处理、渲染管理
- **Player类** - 蛇的移动、增长、碰撞检测
- **Food类** - 食物生成、渲染

### 核心特性

- **60FPS渲染循环** - 流畅的游戏体验
- **Delta Time更新** - 帧率独立的游戏逻辑
- **状态管理** - 清晰的游戏状态控制
- **资源管理** - 自动内存清理

## 自定义游戏

### 修改游戏参数

在 `src/player.h` 中可以调整：
```cpp
Player::Player(float startX, float startY, float size) 
    : ... {
    // 修改蛇的大小
}
```

在 `src/game.h` 中可以调整：
```cpp
private:
    int screenWidth = 800;   // 屏幕宽度
    int screenHeight = 600;  // 屏幕高度
```

### 添加新功能

1. **新游戏对象** - 继承自基本游戏对象类
2. **音效** - 集成SDL2_mixer
3. **图像资源** - 使用SDL2_image加载图片
4. **存档系统** - 添加分数保存功能

## 故障排除

### 编译错误

1. **"找不到CMake"** - 安装CMake并添加到PATH
2. **"找不到编译器"** - 安装Visual Studio或Build Tools
3. **"网络下载失败"** - 检查防火墙设置

### 运行时错误

1. **"SDL初始化失败"** - 确保系统支持DirectX
2. **"窗口创建失败"** - 检查显示器设置
3. **游戏崩溃** - 运行Release版本

### 性能问题

1. **低帧率** - 关闭其他程序，关闭VSync
2. **卡顿** - 检查后台进程
3. **内存泄漏** - 确保正确清理资源

## 扩展建议

1. **添加不同类型的食物** - 特殊效果
2. **障碍物系统** - 静态和移动障碍
3. **多人模式** - 本地多人游戏
4. **关卡系统** - 递增难度
5. **菜单界面** - 开始/设置/高分界面

## 学习资源

- [SDL2官方文档](https://wiki.libsdl.org/)
- [CMake教程](https://cmake.org/cmake/help/latest/guide/tutorial/)
- [C++游戏开发指南](https://www.gamefromscratch.com/page/Game-From-Scratch-CPP-Edition.aspx)

## 许可证

本项目采用MIT许可证，可以自由使用和修改。

---

**享受游戏开发的乐趣！** 🎮✨