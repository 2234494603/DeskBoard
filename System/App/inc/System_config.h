/**
 * @file app_config.h
 * @brief CN：工程级"总管头文件"。设计目标是：**所有 .h 都从这里进**，任何 .c 只写
 *        一行 #include "System_config.h" 就能拿到全部标准库、练习模块、LVGL、FreeRTOS
 *        的声明与开关。main.c 现在就只有这一行包含。
 *        -- EN：The project's umbrella header. Every other header is pulled in
 *        here, so a .c file needs a single #include "System_config.h" to get the C
 *        standard library, the practice modules, LVGL and FreeRTOS.
 */

#ifndef APP_CONFIG     // CN：头文件保护宏，防止同一编译单元内被重复包含。-- EN：Include guard.
#define APP_CONFIG     // CN：定义保护宏，与上面的 #ifndef 配对。-- EN：Define the guard macro.

/*============================================================================
 * 一、C 标准库
 *============================================================================*/
#include <stdio.h>      // CN：printf / snprintf。-- EN：printf / snprintf.
#include <stdlib.h>     // CN：malloc / free / rand。-- EN：malloc / free / rand.
#include <string.h>     // CN：memset / strncpy / memcpy。-- EN：memset / strncpy / memcpy.
#include <stdint.h>     // CN：uint32_t 等定长整数。-- EN：Fixed-width integer types.
#include <stdbool.h>    // CN：bool / true / false。-- EN：bool / true / false.

/*============================================================================
 * 二、本工程的练习模块（List.h / Calc.h 自己也包含本文件，靠保护宏不会递归）
 *============================================================================*/
#include "List.h"       // CN：双向链表接口。-- EN：Doubly linked list interface.
#include "Calc.h"       // CN：算术接口。-- EN：Arithmetic interface.

/*============================================================================
 * 三、应用层：LVGL 界面 + FreeRTOS 演示
 *    lvgl_app.h 里包含 lvgl.h，所以 LVGL 的全部 API 也随之可见。
 *============================================================================*/
#include "lvgl_app.h"   // CN：LVGL 界面入口（内部再包含 lvgl.h）。-- EN：LVGL UI entry (includes lvgl.h).
#include "freertos_app.h" // CN：FreeRTOS 演示模块接口。-- EN：FreeRTOS demo module interface.

/* CN：中文字体（由 System/Drive/TTF/dingdingjinbuti.ttf 用 lv_font_conv 生成，覆盖 GB2312 全字库）。
 *     生成命令见 System/docs/font.md；想换字体/字号就重新生成 System/App/src/font_cn_16.c。
 * EN：The Chinese font (generated from System/Drive/TTF/dingdingjinbuti.ttf with
 * lv_font_conv; full GB2312 coverage). See System/docs/font.md to regenerate it. */
LV_FONT_DECLARE( lv_font_cn_16 );

/*============================================================================
 * 四、FreeRTOS 内核 API（任务 / 队列 / 信号量 / 软件定时器）
 *    第三方库的头文件也统一从这里进，方便以后新加 .c 时直接调用。
 *============================================================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

/*============================================================================
 * 五、全局编译开关
 *============================================================================*/

# define LOG_SWITCH 0  // CN：调试打印总开关，0 表示关闭，所有被 #if LOG_SWITCH 包裹的 printf 都在预处理阶段被剔除。-- EN：Master switch for debug prints; 0 disables them, so every printf guarded by #if LOG_SWITCH is removed during preprocessing.
# define POSITIVE   0  // CN：遍历方向开关，0 使 List.c 编译 #else 分支，即 List_PrintBackward 反向遍历。-- EN：Traversal-direction switch; 0 selects the #else branch in List.c, the backward List_PrintBackward.

/*----------------------------------------------------------------------------
 * 六、界面 / 任务开关（想自己写页面、自己建任务，就动这里）
 *
 *   把某个开关改成 0，对应那块代码在预处理阶段就被剔掉，不参与编译；
 *   全部保留 = 1 时，窗口里有两个标签页：控件演示 / FreeRTOS。
 *
 *   Turn a switch to 0 and that part is removed at preprocessing time. With all
 *   both set to 1 the window shows two tabs.
 *----------------------------------------------------------------------------*/

/* CN：1 = 显示内置的 LVGL 控件演示页（弧形/滑块/折线图）。-- EN：Built-in widget demo tab. */
#define APP_UI_DEMO_WIDGETS 1

/* CN：1 = 显示内置的 FreeRTOS 监视页（任务表/队列/互斥量）。-- EN：Built-in FreeRTOS monitor tab. */
#define APP_UI_RTOS_MONITOR 1


/* CN：启动时显示第几页（0 = 第一页，1 = 第二页）。
 *     超出范围时会自动退回第一页，不会崩。
 * EN：Initial tab index (0 = first, 1 = second). An
 * out-of-range value silently falls back to the first tab. */
#define APP_UI_START_TAB   0

/* CN：只想看其中一个？把另一个改成 0 即可，例如：
 *       #define APP_UI_DEMO_WIDGETS 0
 *       #define APP_UI_RTOS_MONITOR 0
 * EN：Want just one tab? Set the other demo to 0. */

#endif
