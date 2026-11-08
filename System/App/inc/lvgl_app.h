/**
 * @file lvgl_app.h
 * @brief CN：LVGL 界面模块的对外接口。拆成 Init / Step / Run 三个函数，是为了让
 *        "谁来驱动 LVGL"变成可替换的：主线程可以驱动（Run），FreeRTOS 的 GUI
 *        任务也可以驱动（Init + Step）。
 *        -- EN：Public interface of the LVGL UI module. Splitting it into
 *        Init / Step / Run makes "who drives LVGL" pluggable: the main thread
 *        (Run) or a FreeRTOS GUI task (Init + Step).
 */

#ifndef LVGL_APP_H
#define LVGL_APP_H

#include <stdbool.h>
#include <stdint.h>

#include "lvgl.h"   /* CN：拿到 lv_xxx 类型与全部 LVGL API。-- EN：Pull in lv_xxx types and the full LVGL API. */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief CN：只创建窗口（LVGL 内核 + Win32 后端 + 显示），不建任何演示页面。
 *        自己写页面时用它：InitWindow → 自己 Create → Run。
 *        -- EN：Create the window only (core + Win32 backend + display), without
 *        any demo UI. Use it to write your own page.
 */
void Lvgl_App_InitWindow( void );

/**
 * @brief CN：把主题和屏幕的字体换成中文字体（不做这一步，中文会显示成方块）。
 *        如果你自己调用 lv_init() + lv_windows_create_display() 建窗口，
 *        建好之后调用它一次即可。
 *        -- EN：Switch the theme and screen font to the CJK font. Call once after
 *        creating the display yourself.
 * @param display CN：lv_windows_create_display() 返回的显示对象。-- EN：Display handle.
 */
void Lvgl_App_UseChineseFont( lv_display_t * display );

/**
 * @brief CN：初始化 LVGL（内核 + Win32 后端 + 窗口 + 界面）。重复调用只生效一次。
 *        -- EN：Initialize LVGL (core + Win32 backend + window + UI). Idempotent.
 */
void Lvgl_App_Init( void );

/**
 * @brief CN：驱动一次 LVGL（处理定时器、渲染、输入），并返回建议的休眠毫秒数。
 *        在 FreeRTOS 任务里调用后应接着 vTaskDelay(delay_ms)。
 *        -- EN：Run one LVGL iteration (timers, rendering, input) and return the
 *        suggested sleep time in milliseconds.
 *
 * @param delay_ms CN：输出参数，下一次调用前建议休眠的毫秒数。-- EN：Out: suggested delay.
 * @return CN：true 表示界面仍然有效。-- EN：true while the UI is still alive.
 */
bool Lvgl_App_Step( uint32_t * delay_ms );

/**
 * @brief CN：不接 RTOS 的主循环：反复驱动 LVGL，永不返回。
 *        调用前必须先 Lvgl_App_InitWindow() 或 Lvgl_App_Init()。
 *        -- EN：The no-RTOS main loop; never returns. Call Lvgl_App_InitWindow()
 *        or Lvgl_App_Init() first.
 */
void Lvgl_App_Run( void );

#ifdef __cplusplus
}
#endif

#endif /* LVGL_APP_H */
