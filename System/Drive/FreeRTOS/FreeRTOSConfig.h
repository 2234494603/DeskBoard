/**
 * @file FreeRTOSConfig.h
 * @brief CN：FreeRTOS 内核配置。这是"外挂 FreeRTOS"能跑起来的关键文件：
 *        内核的每个 .c 都会包含它，行为完全由这里的宏决定。
 *        -- EN：FreeRTOS kernel configuration. Every kernel .c includes this
 *        file; the macros below define the whole behaviour.
 *
 * CN：本配置配合 portable/MSVC-MingW（Windows 线程模拟的任务切换）+ heap_4 使用，
 *     目的是在 PC 上把 FreeRTOS 的 API 原样跑起来，方便没有开发板时学习。
 * -- EN：This configuration targets portable/MSVC-MingW (task switching simulated
 *     with Windows threads) plus heap_4, so the real FreeRTOS API can be studied
 *     on a PC without a board.
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdio.h>

/*============================================================================
 * 一、调度器基本行为
 *============================================================================*/

/** CN：1 = 抢占式调度（高优先级任务就绪立刻抢占）。-- EN：1 = preemptive scheduling. */
#define configUSE_PREEMPTION                    1

/** CN：Windows 模拟端口不支持"硬件指令找最高优先级任务"，必须为 0。
 *      -- EN：The Windows simulator port cannot use hardware priority lookup; must be 0. */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0

/** CN：低功耗 tickless 需要真实硬件定时器，模拟端口不支持。-- EN：Tickless idle needs real hardware; unsupported here. */
#define configUSE_TICKLESS_IDLE                 0

/** CN：CPU 主频，模拟端口用不到，给个占位值。-- EN：CPU clock is unused by the simulator port. */
#define configCPU_CLOCK_HZ                      ( 1000000000UL )

/** CN：系统节拍 1000Hz，即 1 个 tick = 1ms，pdMS_TO_TICKS() 可以直接换算。
 *      -- EN：1 kHz tick, so one tick equals 1 ms and pdMS_TO_TICKS() is exact. */
#define configTICK_RATE_HZ                      ( ( TickType_t ) 1000 )

/** CN：优先级数量（0 最低，configMAX_PRIORITIES-1 最高）。-- EN：Number of priorities. */
#define configMAX_PRIORITIES                    8

/** CN：空闲任务的栈大小（单位是 StackType_t，不是字节）。
 *      -- EN：Idle task stack depth, in StackType_t units (not bytes). */
#define configMINIMAL_STACK_SIZE                ( ( unsigned short ) 512 )

/** CN：任务名最大长度，超长会被截断。-- EN：Maximum task name length. */
#define configMAX_TASK_NAME_LEN                 16

/** CN：必须为 0：Windows 端口的世界里 TickType_t 是 32 位。
 *      -- EN：Must stay 0; the Windows port uses a 32-bit tick counter. */
#define configUSE_16_BIT_TICKS                  0

/** CN：同优先级任务时间片轮转。-- EN：Round-robin time slicing between equal priorities. */
#define configUSE_TIME_SLICING                  1

/** CN：空闲任务主动让出，避免同优先级饿死。-- EN：Idle task yields to equal-priority tasks. */
#define configIDLE_SHOULD_YIELD                 1

/*============================================================================
 * 二、同步与通信原语（本工程演示用到的都在这里打开）
 *============================================================================*/

#define configUSE_MUTEXES                       1   /* CN：互斥量 xSemaphoreCreateMutex。-- EN：Mutual exclusion. */
#define configUSE_RECURSIVE_MUTEXES             1   /* CN：递归互斥量。-- EN：Recursive mutexes. */
#define configUSE_COUNTING_SEMAPHORES           1   /* CN：计数信号量。-- EN：Counting semaphores. */
#define configUSE_TASK_NOTIFICATIONS            1   /* CN：任务通知（最轻量的同步方式）。-- EN：Task notifications. */
#define configTASK_NOTIFICATION_ARRAY_ENTRIES   3   /* CN：每个任务的通知槽位数量。-- EN：Notification slots per task. */
#define configUSE_QUEUE_SETS                    0   /* CN：队列集，本例不需要。-- EN：Queue sets, unused. */
#define configQUEUE_REGISTRY_SIZE               8   /* CN：可给队列起名字，调试器里能看到。-- EN：Named queues for debugging. */
#define configUSE_CO_ROUTINES                   0   /* CN：协程是老特性，不启用。-- EN：Co-routines disabled. */
#define configMAX_CO_ROUTINE_PRIORITIES         1

/*============================================================================
 * 三、内存：heap_4 从这块静态数组里分配
 *============================================================================*/

#define configSUPPORT_STATIC_ALLOCATION         0   /* CN：只用动态分配，代码更少。-- EN：Dynamic allocation only. */
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( 96 * 1024 ) )  /* CN：96KB 堆。-- EN：96 KB heap. */
#define configUSE_MALLOC_FAILED_HOOK            0   /* CN：分配失败钩子，本例关闭。-- EN：No malloc-failed hook. */
#define configCHECK_FOR_STACK_OVERFLOW          0   /* CN：模拟端口没有真实栈检查。-- EN：No stack-overflow check (simulated stacks). */

/*============================================================================
 * 四、软件定时器（定时器回调运行在"定时器服务任务"里，不是中断）
 *============================================================================*/

#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               2
#define configTIMER_QUEUE_LENGTH                16
#define configTIMER_TASK_STACK_DEPTH            512

/*============================================================================
 * 五、调试 / 统计
 *============================================================================*/

/** CN：打开后才能用 uxTaskGetSystemState() 遍历任务列表（监视页用）。
 *      -- EN：Enables uxTaskGetSystemState(), used by the monitor page. */
#define configUSE_TRACE_FACILITY                1
#define configGENERATE_RUN_TIME_STATS           0
#define configUSE_STATS_FORMATTING_FUNCTIONS    0

#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configUSE_APPLICATION_TASK_TAG          0
#define configUSE_NEWLIB_REENTRANT              0
#define configENABLE_BACKWARD_COMPATIBILITY     0
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 0
#define configSTACK_DEPTH_TYPE                  uint32_t

/*============================================================================
 * 六、按需裁剪的 API（打开后才会被编译进内核）
 *============================================================================*/

#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_xTaskGetIdleTaskHandle          1
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_xTimerPendFunctionCall          1
#define INCLUDE_xTaskAbortDelay                 1
#define INCLUDE_xTaskGetHandle                  1
#define INCLUDE_xTaskResumeFromISR              1

/*============================================================================
 * 七、断言：内核发现非法参数时打印出错位置，方便定位
 *============================================================================*/

#define configASSERT( x )                                   \
    do {                                                    \
        if( ( x ) == 0 ) {                                   \
            printf( "[FreeRTOS] ASSERT: %s:%d\r\n",          \
                    __FILE__, __LINE__ );                    \
            fflush( stdout );                               \
        }                                                    \
    } while( 0 )

#endif /* FREERTOS_CONFIG_H */
