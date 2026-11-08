/**
 * @file led.c
 * @brief CN：你自己模块的实现（示例，可删）。
 *        Makefile 会自动把 User/ 下的 .c 编进来（make user 模式下），不用改任何配置。
 *        -- EN：Implementation of your module (example). Every .c under User/ is
 *        picked up automatically in `make user` mode.
 */

#include "led.h"

static uint8_t s_state;   /* CN：0 = 灭，1 = 亮。-- EN：0 = off, 1 = on. */

void Led_Init( void )
{
    s_state = 0;
}

uint8_t Led_Toggle( void )
{
    s_state = ( s_state == 0u ) ? 1u : 0u;
    return s_state;
}

uint8_t Led_Get( void )
{
    return s_state;
}
