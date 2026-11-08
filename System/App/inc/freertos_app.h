/**
 * @file freertos_app.h
 * @brief CN：FreeRTOS 演示模块对外的接口。头文件故意不包含任何 FreeRTOS 头，
 *        这样界面层（LVGL）只依赖普通 C 结构体，不会被 RTOS 类型污染。
 *        -- EN：Public interface of the FreeRTOS demo module. This header
 *        deliberately includes no FreeRTOS header, so the UI layer only depends
 *        on plain C structs.
 */

#ifndef FREERTOS_APP_H
#define FREERTOS_APP_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * CN：运行统计快照。每个字段由"唯一一个任务"递增，界面任务只做读取，
 *     读取时用临界区保证拿到一组相对一致的值。
 * -- EN：Runtime statistics snapshot. Each field is incremented by exactly one
 * task; the UI task only reads them, taking a critical section so the values
 * are at least mutually consistent.
 */
typedef struct {
    uint32_t heartbeat;      /* CN：心跳任务计数。-- EN：Heartbeat task counter. */
    uint32_t produced;       /* CN：成功写入队列的消息数。-- EN：Messages queued. */
    uint32_t consumed;       /* CN：成功从队列取出的消息数。-- EN：Messages consumed. */
    uint32_t dropped;        /* CN：队列满导致丢弃的次数。-- EN：Dropped because the queue was full. */
    uint32_t queue_depth;    /* CN：当前队列中的消息数。-- EN：Messages currently waiting. */
    uint32_t queue_size;     /* CN：队列容量。-- EN：Queue capacity. */
    uint32_t timer_ticks;    /* CN：软件定时器回调次数。-- EN：Software timer callbacks. */
    uint32_t sem_taken;      /* CN：二值信号量成功获取次数。-- EN：Successful semaphore takes. */
    uint32_t sem_timeout;    /* CN：获取信号量超时次数。-- EN：Semaphore timeouts. */
    uint32_t lock_ok;        /* CN：互斥量保护下的计数。-- EN：Counter guarded by a mutex. */
    uint32_t lock_bad;       /* CN：无保护计数（会出现数据竞争）。-- EN：Unguarded counter (data race). */
    uint32_t tick_ms;        /* CN：当前系统 tick（毫秒）。-- EN：Current system tick in ms. */
    uint32_t heap_free;      /* CN：FreeRTOS 堆剩余字节。-- EN：Free bytes left in the FreeRTOS heap. */
    int32_t  last_value;     /* CN：消费者最后收到的值。-- EN：Last value received by the consumer. */
} rtos_stats_t;

/**
 * CN：任务列表里的一行，供监视页画表格。state 用单个字符表示：
 *     R=运行/Running  r=就绪/Ready  B=阻塞/Blocked  S=挂起/Suspended  D=已删除/Deleted
 * -- EN：One row of the task table. "state" is a single character:
 *     R=running, r=ready, B=blocked, S=suspended, D=deleted.
 */
typedef struct {
    char     name[16];
    char     state;
    uint8_t  priority;
    uint16_t stack_free;   /* CN：栈高水位（剩余最小空闲量）。-- EN：Stack high-water mark. */
} rtos_task_info_t;

/**
 * @brief CN：创建全部演示任务并启动调度器。正常情况下**不会返回**。
 *        -- EN：Create all demo tasks and start the scheduler. Normally never returns.
 */
void FreeRTOS_App_Start( void );

/**
 * @brief CN：读取一份统计快照。可在任意任务里调用。
 *        -- EN：Copy out the current statistics. Safe to call from any task.
 */
void FreeRTOS_GetStats( rtos_stats_t * out );

/**
 * @brief CN：读取当前任务列表（uxTaskGetSystemState 的封装）。
 *        -- EN：Read the current task list (wraps uxTaskGetSystemState).
 * @return CN：实际写入的任务数量。-- EN：Number of rows written.
 */
uint32_t FreeRTOS_GetTaskList( rtos_task_info_t * out, uint32_t max );

/**
 * @brief CN：调度器是否已经跑起来。
 *        -- EN：Whether the scheduler is running.
 */
bool FreeRTOS_IsRunning( void );

#ifdef __cplusplus
}
#endif

#endif /* FREERTOS_APP_H */
