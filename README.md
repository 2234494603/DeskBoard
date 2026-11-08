# DeskBoard

> **DeskBoard = Desk + Board**：把开发板搬到桌面上。没有板子、没有调试器，
> 也能练 LVGL 界面和 FreeRTOS 调度。

用 **LVGL 9 + FreeRTOS V11.3.1** 在 Windows 上练手的小工程：不买开发板、不装 Visual Studio、
不用 CMake，只要 MSYS2 的 gcc + make，就能看到真实的图形界面和真实的 RTOS 调度。

界面全中文（自带字体），并且专门给你留了一块地方写自己的界面。

## 目录结构

工程分 **User**（你写）和 **System**（系统提供）两层：

```
User/                      ← 你的地盘，随便改
├── main.c                   程序入口：建窗口 → 搭页面 → 进主循环
├── led.h / led.c            你自己模块的示例（可删）
└── （新加的 .c 会自动参与编译）

System/                    ← 系统提供，一般不用动
├── System_main.c            系统演示入口（make 时用这个）
├── App/inc/                 框架头文件：System_config.h（总开关）、lvgl_app.h …
├── App/src/                 框架实现：lvgl_app.c、freertos_app.c、字体
├── docs/                    截图、字体说明、Makefile 逐行解释
└── Drive/
    ├── DataStructures/      练习模块（Calc 算术、List 链表）
    ├── LVGL_GUI/            LVGL 9 源码
    ├── FreeRTOS/            FreeRTOS 内核 + Windows 模拟端口
    └── TTF/                 原始字体文件
```

`System/App/inc/System_config.h` 是全工程唯一入口头文件：标准库、List/Calc、LVGL、FreeRTOS、
以及所有开关都在里面。

## 环境搭建（0 基础，10 分钟）

1. 装 **MSYS2**（官网 <https://www.msys2.org/>，一路默认即可，默认在 `C:\msys64`）。
2. 打开 **MSYS2 UCRT64** 终端，装编译器和 make：

   `pacman -S --needed mingw-w64-ucrt-x86_64-gcc make`

3. 把 `C:\msys64\ucrt64\bin `
4. 加进系统 `Path`，**重新打开** PowerShell。
5. 验证：`gcc --version` 和 `make --version` 都能输出版本号。

> 本机实际装在 `C:\GCC\msys64`，路径按自己的情况替换。

## 编译运行

| 命令 | 产物 | 说明 |
| --- | --- | --- |
| `make -j8` | `DeskBoard.exe` | 系统演示：两个标签页的完整例子 |
| `make user` | `DeskBoard_user.exe` | 编译你的 `User/main.c` **并直接运行** |
| `make run` | — | 编译并运行对应入口 |
| `make clean` | — | 清掉 `build/` 和两个 exe |

两个入口各生成一个 exe，互不覆盖，切换时不用先 clean。首次编译约 1 分钟（LVGL 有 480 个 .c），
之后改自己的代码只重编改动的文件。

## 写你自己的第一个界面

`User/main.c` 是**你自己的文件**，里面每一步都有中文注释：

1. `lv_init()` —— 初始化 LVGL 内核
2. `lv_windows_create_display(标题, 宽, 高, 缩放%, 允许DPI覆盖, 模拟器模式)` ——
   用 **LVGL 的 Win32 原生接口**建窗口。改第 2、3 个参数就能改屏幕大小；
   最后两个参数写 `false`，窗口就能拖边框缩放、最大化
3. `Lvgl_App_UseChineseFont(display)` —— 一行让中文正常显示
4. 往 `lv_screen_active()` 上加控件（标签、按钮、滑块……）
5. `Lvgl_App_Run()` —— 进主循环，永不返回

`main.c` 里只放了一个标题、一句提示、一个按钮作示范，删掉换成自己的即可；
文件末尾还列了 5 个「试一试」的小练习。

改完在项目根目录输入：

```
make user
```

它会**自动编译并弹出窗口**（窗口关掉后命令才结束）。

## 加自己的文件 / 暴露接口

| 想做的事 | 怎么做 |
| --- | --- |
| 加一个自己的 .c | 放进 `User/`，`make user` 自动编译，**不用改 Makefile** |
| 加一个框架模块 | 放进 `System/App/src/`，头文件放 `System/App/inc/` |
| 新建独立模块目录 | 才需要在 Makefile 里加一行 `wildcard` 和一行 `-I` |
| 让别的文件能调用你的函数 | 函数在 .c 实现、在 .h 声明，调用方 `#include "你的头文件.h"` |

`User/led.h` + `User/led.c` 就是现成例子：头文件只有声明，`User/main.c` 里 include 后调用 `Led_Init()`。

## 界面预览

| 标签页 | 内容 |
| --- | --- |
| `控件演示` | 弧形进度条、滑块、进度条、折线图、按钮 |
| `FreeRTOS` | 心跳、队列收发与丢弃、软件定时器、二值信号量、互斥量 vs 数据竞争、任务表 |

> 这两页只在演示版（`make`）里出现。你自己写界面时（`make user`）屏幕是空的，
> 上面画什么完全由 `User/main.c` 决定。

## 界面开关

都在 `System_config.h` 第六节，改完重编即可：

| 开关 | 默认 | 作用 |
| --- | --- | --- |
| `APP_UI_DEMO_WIDGETS` | 1 | 显示"控件演示"页 |
| `APP_UI_RTOS_MONITOR` | 1 | 显示"FreeRTOS"监视页 |
| `APP_UI_START_TAB` | 0 | 启动时显示第几页（0/1） |

> 这两个开关只影响 `make` 生成的演示版；`make user` 用的是你 `User/main.c` 里的界面，与它们无关。

## 中文字体

LVGL 自带的 Montserrat 没有中文，自带的思源黑体子集又只有 1195 个字（`演`、`页` 都缺）。
所以用 `lv_font_conv` 把 `System/Drive/TTF/dingdingjinbuti.ttf` 转成了
`System/App/src/font_cn_16.c`（16px、4bpp、GB2312 全字库），界面文字自动继承它。

换字体、换字号、重新生成的步骤见 [System/docs/font.md](System/docs/font.md)。

## FreeRTOS 演示了哪些 API

| 任务 | 优先级 | 演示内容 |
| --- | --- | --- |
| `lvgl` | 4 | 在 RTOS 任务里驱动 `lv_timer_handler()` |
| `consumer` | 3 | `xQueueReceive(..., portMAX_DELAY)` 阻塞等待 |
| `producer` | 2 | `xQueueSend(..., 0)` 非阻塞发送，队列满即丢弃 |
| `sem` | 2 | 二值信号量 + 超时计数 |
| `heartbeat` | 1 | `vTaskDelayUntil()` 固定周期 |
| `mutex_a/b` | 1 | 互斥量保护共享计数（结果准确） |
| `race_a/b` | 1 | 同样逻辑但不加锁，演示数据竞争丢更新 |

另外还有软件定时器（回调跑在定时器服务任务里）、`uxTaskGetSystemState()` 任务表、
`xPortGetFreeHeapSize()` 堆余量。

## 原理速览

- **LVGL**：`lv_conf.h` 里 `LV_USE_OS = LV_OS_WINDOWS` + `LV_USE_WINDOWS = 1`，直接用 LVGL 自带的
  Win32 后端建窗口、画像素、收鼠标键盘，所以不需要 SDL。
- **FreeRTOS**：`System/Drive/FreeRTOS/portable/MSVC-MingW/port.c` 是官方 Windows 模拟端口，
  每个任务对应一个 Windows 线程，用 `timeBeginPeriod()` 把定时精度提到 1ms（链接 `-lwinmm`）。
- **入口**：`main()` → 建窗口 → `FreeRTOS_App_Start()` 创建任务 → `vTaskStartScheduler()`，
  正常永不返回；界面由 `lvgl` 任务刷新。

⚠️ 该端口会把进程优先级提到 `REALTIME_PRIORITY_CLASS` 并绑到 0 号核。任务都会主动让出 CPU，
桌面能正常用，但别在里面写死循环。

## 学习资源

- **MSYS2 官网**：<https://www.msys2.org/>
- **韦东山 FreeRTOS 教程**：<https://rtos.100ask.net/zh/FreeRTOS/DShanMCU-F103/>
- **韦东山 LVGL 教程（8.1 版）**：<https://lvgl.100ask.net/8.1/index.html>

> 本仓库用 LVGL **9.x**，与教程的 8.1 有 API 差异（`lv_btn_create` → `lv_button_create`、
> `LV_COLOR_DEPTH` → `LV_COLOR_FORMAT_DEFAULT` 等）。看教程以思路为主，函数名以源码为准。

## 常见问题

| 现象 | 原因 / 解决 |
| --- | --- |
| `make` 提示找不到 gcc | MSYS2 的 `ucrt64\bin` 没进 PATH，或 PATH 里有旧版 gcc 排在前面 |
| 中文显示成方块 | 字体没覆盖到，重新生成 `font_cn_16.c`（见 `System/docs/font.md`） |
| 双击 exe 没反应 | `DeskBoard_user.exe` 是你的入口，`main` 里没建窗口就会直接退出 |
| 改了开关不生效 | 开关在 `System_config.h`，改完要重新 `make` |
| 想让界面开机就是自己那页 | `APP_UI_START_TAB` 改成 2 |

## 其它文档

- [System/docs/font.md](System/docs/font.md)：中文字体生成与替换
- [System/docs/解释脚本.txt](System/docs/解释脚本.txt)：Makefile 逐行解释
- [GIT_RULES.md](GIT_RULES.md)：本仓库的提交规范
