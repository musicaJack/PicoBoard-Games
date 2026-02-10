/*****************************************************************************
* | File      	:   Readme_CN.txt
* | Author      :   
* | Function    :   Help with use
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2021-02-04
* | Info        :   在这里提供一个中文版本的使用文档，以便你的快速使用
******************************************************************************/
这个文件是帮助您使用本例程。
本工程已精简为 **仅保留 1.3 英寸 LCD** 所需的驱动与硬件配置（Config + LCD_1in3）。
在这里简略的描述本工程的使用：

1.基本信息：
本例程使用相对应的模块搭配Pico进行了验证，你可以在工程的examples\中查看对应的测试例程;

2.管脚连接：

所有引脚以原厂「引脚排列介绍」为准，统一在 src\drivers\hw\BoardPins.h 中配置：

LCD：RST=GP12, DC=GP8, CS=GP9, CLK=GP10, DIN=GP11, BL=GP13
摇杆：UP=GP2, DOWN=GP18, LEFT=GP16, RIGHT=GP20, CTRL(中间)=GP3
按键：A=GP15, B=GP17, X=GP19, Y=GP21

3.基本使用：
推荐使用 Ninja（Windows 下更省心）：
    cmake -S . -B build -G Ninja
    cmake --build build

生成的 .uf2 在 build 目录下（文件名通常为 main.uf2），复制到 Pico 盘符即可。

4.主菜单与游戏操作：
- 主菜单：上下选择游戏，A 或 中间键(CTRL) 进入；井字棋 / 五子棋
- 游戏中：方向键移动，A/Y/中间键 落子，B 重开，X 返回主菜单

5.目录结构（精简版）：
src\app\:           应用入口，app_main.c 做初始化与主循环
src\ui\:            菜单与游戏界面（menu_ui、tictactoe_ui、gomoku_ui：显示与按键）
src\game\:          井字棋/五子棋的纯逻辑（棋盘状态、胜负判定、Alpha-Beta AI）
src\core\:          通用输入（按键去抖）与渲染（帧缓冲、画线/圆、5×7 字体）
src\drivers\hw\:    硬件抽象；BoardPins.h 为引脚统一配置（原厂引脚）
src\drivers\lcd\:   1.3 英寸 LCD 驱动

6.分层架构说明：
依赖方向为：app → ui → game、core；ui → drivers；core → drivers。
- drivers：板级 PIN、SPI/I2C/GPIO 与 LCD 驱动，不依赖业务。
- core：统一 Button 输入与 FrameBuffer 绘图，供 UI 与游戏界面复用。
- game：井字棋/五子棋的棋盘状态、落子规则、赢法判定与 AI，无 UI 与硬件依赖。
- ui：主菜单、井字棋界面、五子棋界面；负责光标、按键和调用 game 接口与 core 渲染。
- app：main 仅做 DEV/LCD 初始化与菜单循环，根据选择调用对应 UI 入口。