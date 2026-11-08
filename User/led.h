/**
 * @file led.h
 * @brief CN：你自己模块的接口（示例，可删）。
 *        规则：函数在 .c 里实现，在 .h 里声明；别的文件 include 这个头就能用。
 *        -- EN：Your module's interface (example, safe to delete). Declare in
 *        the header, implement in the .c.
 */

#ifndef LED_H
#define LED_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** CN：初始化（这里只是把计数清零，实际项目里可能是配 GPIO）。-- EN：Init. */
void Led_Init( void );

/** CN：翻转一次，返回翻转后的状态（0/1）。-- EN：Toggle, returns the new state. */
uint8_t Led_Toggle( void );

/** CN：当前状态。-- EN：Current state. */
uint8_t Led_Get( void );

#ifdef __cplusplus
}
#endif

#endif /* LED_H */
