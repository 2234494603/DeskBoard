/**
 * @file lv_conf.h
 * @brief CN：LVGL 在本工程（Windows + gcc）下的配置文件。这里只写“需要覆盖”的
 *        选项，没有写到的项全部由 lv_conf_internal.h 自动填默认值，所以文件很短。
 *        -- EN：LVGL configuration for this Windows + gcc project. Only the
 *        overridden options are listed; everything else gets a default from
 *        lv_conf_internal.h, which keeps this file short.
 */

#ifndef LV_CONF_H
#define LV_CONF_H

/*====================================================================
 * 一、基础：颜色深度与内存
 *====================================================================*/

/** CN：Win32 后端的帧缓冲是 32 位像素（XRGB8888），所以像素格式必须是 XRGB8888。
 *      等价于旧写法 LV_COLOR_DEPTH 32，但 LVGL 9.5 起推荐用新的写法（旧写法会告警）。
 *      -- EN：The Win32 backend flushes a 32-bit XRGB8888 buffer, so the pixel
 *      format must be XRGB8888. Same as the old LV_COLOR_DEPTH 32, but the new
 *      name is what LVGL 9.5+ expects (the old one emits a deprecation warning). */
#define LV_COLOR_FORMAT_DEFAULT LV_COLOR_FORMAT_XRGB8888

/** CN：LVGL 内置堆大小（字节）。界面里的控件、图片缓冲、字体缓存都从这里分配。
 *      -- EN：Size of LVGL's internal heap in bytes; widgets and caches come from it. */
#define LV_MEM_SIZE (2 * 1024 * 1024U)

/*====================================================================
 * 二、平台后端：这是“让 Windows 跑起来”的关键两行
 *====================================================================*/

/** CN：把 LVGL 的操作系统抽象层切换到 Windows 实现（线程、互斥量、延时全部
 *      直接调用 Win32 API），不需要 FreeRTOS / pthread / SDL。
 *      -- EN：Switch LVGL's OSAL to the Win32 implementation (threads, mutex and
 *      delays call Win32 directly); FreeRTOS / pthread / SDL are not needed. */
#define LV_USE_OS LV_OS_WINDOWS

/** CN：打开 LVGL 自带的 Windows 驱动：创建一个真正的窗口，并把鼠标、键盘、
 *      滚轮事件翻译成 LVGL 输入设备。必须先满足 LV_USE_OS == LV_OS_WINDOWS。
 *      -- EN：Enable LVGL's own Windows driver: a real Win32 window plus mouse /
 *      keyboard / wheel input devices. Requires LV_USE_OS == LV_OS_WINDOWS. */
#define LV_USE_WINDOWS 1

/*====================================================================
 * 三、字体（界面里用到的字号）
 *====================================================================*/

#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_28 1

/* CN：LVGL 自带的中文字体只覆盖 1195 个常用字（"演/页/端"都会缺字），
 *      所以本工程改成用 lv_font_conv 生成的全字库 App/font_cn_16.c，
 *      这里把内置中文字体关掉，省掉约 2MB 的代码。
 * EN：LVGL's bundled CJK font only has 1195 glyphs (missing common characters),
 * so this project uses a generated App/font_cn_16.c instead; the bundled one is
 * disabled to save about 2 MB. */
#define LV_FONT_SOURCE_HAN_SANS_SC_14_CJK 0
#define LV_FONT_SOURCE_HAN_SANS_SC_16_CJK 0

#define LV_FONT_DEFAULT &lv_font_montserrat_14   /* CN：LVGL 内部的兜底字体；界面实际用的是 App/font_cn_16.c。-- EN：Internal fallback; the UI uses App/font_cn_16.c. */

/*====================================================================
 * 四、调试输出：先关掉，需要看 LVGL 日志时改成 1 并设置打印回调
 *====================================================================*/

#define LV_USE_LOG 0

#endif /* LV_CONF_H */
