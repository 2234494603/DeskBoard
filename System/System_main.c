/**
 * @file main.c
 * @brief CN：程序入口文件，把 Calc 算术模块与 List 链表模块串联起来做联调验证。
 *        -- EN：Entry file that ties the Calc arithmetic module and the List
 *        module together for integration testing.
 */

#include "System_config.h"   // CN：全工程唯一入口头文件，标准库 / List / Calc / LVGL / FreeRTOS 全在里面。-- EN：The single umbrella header: C library, List, Calc, LVGL and FreeRTOS.

#if defined( _WIN32 )
    #include <windows.h>   // CN：只为 SetConsoleOutputCP()。-- EN：Only for SetConsoleOutputCP().
#endif


/**
 * @brief CN：程序入口。先调用 add(1,1) 验证算术模块能被正确链接并返回值，再把
 *        一个链表节点指针的地址交给 List_Iint() 做初始化，随后立即返回；函数
 *        本身不实现业务流程，只承担模块联调入口的作用。
 *        -- EN：Program entry. It calls add(1,1) to verify that the arithmetic
 *        module links and returns a value, then hands the address of a list
 *        node pointer to List_Iint() for initialization and returns at once;
 *        it implements no business flow and serves only as an integration entry.
 *
 * @param CN：无参数。
 *        -- EN：No parameters.
 *
 * @return CN：固定返回 0，只表示进程正常退出；模块初始化是否成功不会通过该
 *         返回值反馈给调用者。
 *         -- EN：Always returns 0, meaning only that the process exited
 *         normally; whether module initialization succeeded is not reported.
 */
int main()
{
#if defined( _WIN32 )
    /* CN：控制台输出切到 UTF-8，否则 printf 的中文会乱码（源码和执行字符集都是 UTF-8）。
     * EN：Switch the console to UTF-8 so Chinese printf output is readable. */
    SetConsoleOutputCP( CP_UTF8 );
#endif

    List_Struct node ;  // CN：声明链表节点指针并初始化为空，作为 List_Iint() 的初始化对象。-- EN：Declare a list node pointer initialized to NULL as the target of List_Iint().
    printf("%d\n",add(1,1) );  // CN：调用加法函数计算 1+1 并打印结果，用最小用例确认 Calc 模块已正确链接。-- EN：Call add to compute 1+1 and print it, confirming with the smallest case that the Calc module is linked.

    List_Iint(&node , 0);      // CN：把节点指针的地址传给初始化函数，由其写入数据域并把前驱、后继清空。-- EN：Pass the address of the node pointer to the init function, which stores the value and clears the prev/next links.

    // CN：创建 LVGL 窗口 + 全部 FreeRTOS 任务，然后启动调度器；正常情况下这一行永远不返回。
    // EN：Create the LVGL window plus all FreeRTOS tasks, then start the scheduler;
    //     under normal conditions this call never returns.
    FreeRTOS_App_Start();

    Lvgl_App_Run();            // CN：兜底：如果 FreeRTOS 调度器没能启动，就用主线程直接驱动 LVGL。-- EN：Fallback: if the FreeRTOS scheduler failed to start, drive LVGL from the main thread.

    return 0;                  // CN：返回 0，结束进程。-- EN：Return 0 and terminate the process.
}
