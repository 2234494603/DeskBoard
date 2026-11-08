/**
 * @file freertos_app.c
 * @brief CN：FreeRTOS 演示模块。用真实的 FreeRTOS 内核（V11.3.1）在 Windows 上
 *        跑起 7 个任务 + 1 个软件定时器，把任务、队列、互斥量、二值信号量、
 *        软件定时器这些 API 的运作方式直接"演"出来，界面由 LVGL 任务实时刷新。
 *        -- EN：FreeRTOS demo module. It runs the real FreeRTOS kernel (V11.3.1)
 *        on Windows with 7 tasks plus a software timer, and demonstrates tasks,
 *        queues, mutexes, binary semaphores and software timers live, while an
 *        LVGL task refreshes the UI.
 *
 * CN：任务一览（优先级数字越大越高）
 *     lvgl      4  刷新界面（调用 lv_timer_handler）
 *     consumer  3  阻塞在队列上，收到消息后处理
 *     producer  2  每 150ms 往队列写一条，队列满就丢弃
 *     sem       2  等待二值信号量（模拟"中断通知任务"）
 *     heartbeat 1  每 200ms 加一次心跳
 *     mutex_a/b 1  用互斥量保护共享计数（结果准确）
 *     race_a/b  1  故意不加保护（结果会丢更新，演示数据竞争）
 * -- EN：Task list (higher number = higher priority) as above.
 */

#include "System_config.h"   /* CN：唯一入口头文件（含 FreeRTOS / LVGL / 用户任务）。-- EN：The umbrella header. */

/*********************
 *      DEFINES
 *********************/

#define DEMO_QUEUE_LENGTH    4      /* CN：队列深度。-- EN：Queue depth. */
#define PRODUCER_PERIOD_MS   150    /* CN：生产周期。-- EN：Producer period. */
#define CONSUMER_PERIOD_MS   400    /* CN：消费周期（比生产慢，故意让队列堆积）。-- EN：Consumer is slower on purpose. */
#define HEARTBEAT_PERIOD_MS  200    /* CN：心跳周期。-- EN：Heartbeat period. */
#define SEM_TIMEOUT_MS       1500   /* CN：等信号量的超时时间。-- EN：Semaphore timeout. */

/**********************
 *  STATIC VARIABLES
 **********************/

static QueueHandle_t     s_queue;       /* CN：生产者/消费者之间的队列。-- EN：Producer/consumer queue. */
static SemaphoreHandle_t s_mutex;       /* CN：保护共享计数的互斥量。-- EN：Mutex guarding a shared counter. */
static SemaphoreHandle_t s_binary_sem;  /* CN：模拟中断通知的二值信号量。-- EN：Binary semaphore, like an ISR notification. */
static TimerHandle_t     s_soft_timer;  /* CN：1s 周期的软件定时器。-- EN：1 s software timer. */

static rtos_stats_t s_stats;            /* CN：统计快照的数据源。-- EN：Statistics source. */
static bool         s_running;          /* CN：调度器是否已启动。-- EN：Scheduler running flag. */

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * @brief CN：LVGL 任务：不断驱动 LVGL 的定时器（渲染、动画、输入），
 *        并让出 CPU 给其它任务。这就是嵌入式中"GUI 任务"的典型写法。
 *        -- EN：The LVGL task: keep driving LVGL timers (render, animation,
 *        input) and yield to the other tasks.
 */
static void vLvglTask( void * arg )
{
    uint32_t delay_ms = 5;
    ( void ) arg;

    for( ;; ) {
        ( void ) Lvgl_App_Step( &delay_ms );

        /* CN：vTaskDelay 让出 CPU，delay_ms 由 LVGL 根据下一个定时器算出。
         * EN：vTaskDelay yields the CPU; delay_ms comes from LVGL's next timer. */
        vTaskDelay( pdMS_TO_TICKS( delay_ms ) );
    }
}

/**
 * @brief CN：心跳任务：每 200ms 加一次，用来观察任务是否被"准时"调度。
 *        -- EN：Heartbeat task: ticks every 200 ms, showing periodic scheduling.
 */
static void vHeartbeatTask( void * arg )
{
    TickType_t last_wake = xTaskGetTickCount();
    ( void ) arg;

    for( ;; ) {
        s_stats.heartbeat++;

        if( ( s_stats.heartbeat % 5u ) == 0u ) {
            printf( "[heartbeat] tick=%4u ms  heartbeat=%u\r\n",
                    ( unsigned ) xTaskGetTickCount(),
                    ( unsigned ) s_stats.heartbeat );
        }

        /* CN：vTaskDelayUntil 按固定周期唤醒，不受任务执行时间影响（比 vTaskDelay 更准）。
         * EN：vTaskDelayUntil keeps a fixed period regardless of execution time. */
        vTaskDelayUntil( &last_wake, pdMS_TO_TICKS( HEARTBEAT_PERIOD_MS ) );
    }
}

/**
 * @brief CN：生产者：每 150ms 往队列发一条消息。队列满时用 0 超时立即返回失败，
 *        这样不会阻塞生产者，界面任务也不会被拖住。
 *        -- EN：Producer: sends one message every 150 ms. A zero timeout makes
 *        the send fail immediately when the queue is full instead of blocking.
 */
static void vProducerTask( void * arg )
{
    uint32_t value = 0;
    ( void ) arg;

    for( ;; ) {
        value++;

        if( xQueueSend( s_queue, &value, 0 ) == pdPASS ) {
            s_stats.produced++;

            /* CN：每成功发 5 条就给一次二值信号量，等价于"外设中断里 give，任务里 take"。
             * EN：Give the binary semaphore every 5 messages, mimicking an ISR. */
            if( ( s_stats.produced % 5u ) == 0u ) {
                xSemaphoreGive( s_binary_sem );
            }
        }
        else {
            s_stats.dropped++;
        }

        vTaskDelay( pdMS_TO_TICKS( PRODUCER_PERIOD_MS ) );
    }
}

/**
 * @brief CN：消费者：阻塞在 xQueueReceive 上，队列没数据时任务处于 Blocked 状态，
 *        完全不占 CPU —— 这就是 RTOS 相比"轮询"最大的价值。
 *        -- EN：Consumer: blocks in xQueueReceive, so it consumes no CPU while
 *        the queue is empty.
 */
static void vConsumerTask( void * arg )
{
    ( void ) arg;

    for( ;; ) {
        uint32_t value = 0;

        if( xQueueReceive( s_queue, &value, portMAX_DELAY ) == pdPASS ) {
            s_stats.last_value = ( int32_t ) value;
            s_stats.consumed++;

            /* CN：故意处理得慢一点，让生产者把队列塞满，界面能看到 depth 变化。
             * EN：Processing is slower than producing, so the queue fills up. */
            vTaskDelay( pdMS_TO_TICKS( CONSUMER_PERIOD_MS ) );
        }
    }
}

/**
 * @brief CN：信号量任务：带超时地等待二值信号量，成功和超时分别计数。
 *        -- EN：Semaphore task: waits with a timeout and counts both outcomes.
 */
static void vSemaphoreTask( void * arg )
{
    ( void ) arg;

    for( ;; ) {
        if( xSemaphoreTake( s_binary_sem, pdMS_TO_TICKS( SEM_TIMEOUT_MS ) ) == pdPASS ) {
            s_stats.sem_taken++;
        }
        else {
            s_stats.sem_timeout++;
        }
    }
}

/**
 * @brief CN：互斥量演示任务（两个实例）。每轮做一次"读-改-写"：
 *        取锁 → 读 → 故意延时放大窗口 → 写 → 放锁。
 *        两个任务轮流抢锁，互相排斥，所以计数一个都不会丢。
 *        -- EN：Mutex demo task (two instances). Each round does one
 *        read-modify-write inside the mutex; the two tasks exclude each other,
 *        so no increment is lost.
 */
static void vMutexTask( void * arg )
{
    ( void ) arg;

    for( ;; ) {
        if( xSemaphoreTake( s_mutex, portMAX_DELAY ) == pdPASS ) {
            uint32_t temp = s_stats.lock_ok;

            /* CN：真实工程里临界区应该尽量短；这里故意延时 1ms 把竞争窗口放大，
             *     否则在模拟器上几乎看不出差别。（教学用途，勿照抄到产品里）
             * EN：A real critical section should be short; the 1 ms delay widens
             * the race window on purpose so the demo is visible. */
            vTaskDelay( 1 );
            s_stats.lock_ok = temp + 1;

            xSemaphoreGive( s_mutex );
        }

        vTaskDelay( pdMS_TO_TICKS( 20 ) );
    }
}

/**
 * @brief CN：数据竞争演示任务（两个实例）。完全不加保护地做"读-改-写"：
 *        两个任务会在同一时刻读到同一个旧值，然后都写回"旧值+1"，
 *        于是有一次加法凭空消失 —— 这就是没有互斥量时最常见的 bug。
 *        -- EN：Data-race demo task (two instances). The read-modify-write is not
 *        protected at all: both tasks read the same old value and both write
 *        "old + 1", so one increment silently disappears.
 */
static void vRaceTask( void * arg )
{
    ( void ) arg;

    for( ;; ) {
        uint32_t temp = s_stats.lock_bad;
        vTaskDelay( 1 );                    /* CN：制造上下文切换点。-- EN：Force a context switch here. */
        s_stats.lock_bad = temp + 1;
        vTaskDelay( pdMS_TO_TICKS( 20 ) );
    }
}

/**
 * @brief CN：软件定时器回调。注意：它运行在"定时器服务任务"里，不是中断里，
 *        所以可以调用会阻塞的 API，但也不能在里面做耗时操作。
 *        -- EN：Software timer callback. It runs in the timer service task, not
 *        in an interrupt, so blocking APIs are allowed but must stay short.
 */
static void vSoftwareTimerCallback( TimerHandle_t timer )
{
    ( void ) timer;
    s_stats.timer_ticks++;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void FreeRTOS_App_Start( void )
{
    /* CN：先把 LVGL 窗口和界面建好（调用任何 FreeRTOS API 之前）。
     * EN：Build the LVGL window/UI first, before any FreeRTOS API is used. */
    Lvgl_App_Init();

    s_queue      = xQueueCreate( DEMO_QUEUE_LENGTH, sizeof( uint32_t ) );
    s_mutex      = xSemaphoreCreateMutex();
    s_binary_sem = xSemaphoreCreateBinary();
    s_soft_timer = xTimerCreate( "soft_timer", pdMS_TO_TICKS( 1000 ), pdTRUE, NULL, vSoftwareTimerCallback );

    if( ( s_queue == NULL ) || ( s_mutex == NULL ) || ( s_binary_sem == NULL ) || ( s_soft_timer == NULL ) ) {
        printf( "[rtos] create IPC objects failed\r\n" );
        return;
    }

    /* CN：栈深度参数在 Windows 模拟端口里只影响 FreeRTOS 堆的占用，真实栈由线程负责。
     * EN：In the Windows simulator the stack depth only affects heap usage; the
     * real stack belongs to the Windows thread. */
    xTaskCreate( vLvglTask,      "lvgl",      2048, NULL,            4, NULL );
    xTaskCreate( vConsumerTask,  "consumer",   512, NULL,            3, NULL );
    xTaskCreate( vProducerTask,  "producer",   512, NULL,            2, NULL );
    xTaskCreate( vSemaphoreTask, "sem",        512, NULL,            2, NULL );
    xTaskCreate( vHeartbeatTask, "heartbeat",  512, NULL,            1, NULL );
    xTaskCreate( vMutexTask,     "mutex_a",    512, NULL,            1, NULL );
    xTaskCreate( vMutexTask,     "mutex_b",    512, NULL,            1, NULL );
    xTaskCreate( vRaceTask,      "race_a",     512, NULL,            1, NULL );
    xTaskCreate( vRaceTask,      "race_b",     512, NULL,            1, NULL );

    xTimerStart( s_soft_timer, 0 );

    printf( "[rtos] tasks created, heap free = %u bytes\r\n",
            ( unsigned ) xPortGetFreeHeapSize() );
    printf( "[rtos] starting scheduler ...\r\n" );
    fflush( stdout );

    s_running = true;
    vTaskStartScheduler();     /* CN：正常情况下永远不返回。-- EN：Normally never returns. */

    s_running = false;
    printf( "[rtos] scheduler returned unexpectedly (heap too small?)\r\n" );
}

void FreeRTOS_GetStats( rtos_stats_t * out )
{
    if( out == NULL ) {
        return;
    }

    if( !s_running ) {
        memset( out, 0, sizeof( *out ) );
        return;
    }

    /* CN：进入临界区，保证界面拿到的是"同一时刻"的一组数据。
     *     各计数由不同任务写、互不重叠，所以写侧不需要加锁。
     * EN：Take a critical section so the UI sees one consistent snapshot.
     * Each counter has a single writer, so writers need no lock. */
    taskENTER_CRITICAL();
    *out = s_stats;
    out->queue_depth = ( uint32_t ) uxQueueMessagesWaiting( s_queue );
    out->queue_size  = DEMO_QUEUE_LENGTH;
    out->tick_ms     = ( uint32_t ) xTaskGetTickCount();
    out->heap_free   = ( uint32_t ) xPortGetFreeHeapSize();
    taskEXIT_CRITICAL();
}

uint32_t FreeRTOS_GetTaskList( rtos_task_info_t * out, uint32_t max )
{
    TaskStatus_t status[ 14 ];
    uint32_t count;
    uint32_t i;

    if( ( out == NULL ) || ( max == 0u ) || !s_running ) {
        return 0;
    }

    count = ( uint32_t ) uxTaskGetSystemState( status, 14, NULL );
    if( count > max ) {
        count = max;
    }

    for( i = 0; i < count; i++ ) {
        memset( &out[ i ], 0, sizeof( out[ i ] ) );
        strncpy( out[ i ].name, status[ i ].pcTaskName, sizeof( out[ i ].name ) - 1u );
        out[ i ].priority   = ( uint8_t ) status[ i ].uxCurrentPriority;
        out[ i ].stack_free = ( uint16_t ) status[ i ].usStackHighWaterMark;

        switch( status[ i ].eCurrentState ) {
            case eRunning:   out[ i ].state = 'R'; break;
            case eReady:     out[ i ].state = 'r'; break;
            case eBlocked:   out[ i ].state = 'B'; break;
            case eSuspended: out[ i ].state = 'S'; break;
            case eDeleted:   out[ i ].state = 'D'; break;
            default:         out[ i ].state = '?'; break;
        }
    }

    /* CN：按优先级从高到低排一下，界面看起来更符合"调度器视角"。
     * EN：Sort by priority (high first) so the table reads like the scheduler sees it. */
    for( i = 1; i < count; i++ ) {
        rtos_task_info_t key = out[ i ];
        uint32_t j = i;
        while( ( j > 0u ) && ( out[ j - 1u ].priority < key.priority ) ) {
            out[ j ] = out[ j - 1u ];
            j--;
        }
        out[ j ] = key;
    }

    return count;
}

bool FreeRTOS_IsRunning( void )
{
    return s_running;
}
