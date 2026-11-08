/**
 * @file lvgl_app.c
 * @brief CN：LVGL 框架代码（一般不用改）。它负责初始化 LVGL、用自带的 Win32 驱动
 *        建窗口，并按 System_config.h 的开关挂标签页：
 *          APP_UI_DEMO_WIDGETS → "控件演示"
 *          APP_UI_RTOS_MONITOR → "FreeRTOS"
 *        -- EN：LVGL framework code. It initializes LVGL, creates the window with
 *        the built-in Win32 driver and adds tabs according to System_config.h.
 */

#include "System_config.h"

#if defined( _WIN32 )
    #include <windows.h>   /* CN：SetForegroundWindow / SetWindowPos。-- EN：Win32 window helpers. */
#endif

/*********************
 *      DEFINES
 *********************/

#define WINDOW_W 800   /* CN：窗口客户区宽度（像素）。-- EN：Client area width. */
#define WINDOW_H 480   /* CN：窗口客户区高度（像素）。-- EN：Client area height. */

/**********************
 *  STATIC VARIABLES
 **********************/

static bool s_window_inited;  /* CN：窗口是否已创建。-- EN：Window created? */
static bool s_ui_built;       /* CN：演示页面是否已搭好。-- EN：Demo UI built? */

/*============================================================================
 *  演示页 1：控件演示（APP_UI_DEMO_WIDGETS）
 *============================================================================*/
#if APP_UI_DEMO_WIDGETS

static lv_obj_t * s_arc;              /* CN：弧形进度条。-- EN：Arc gauge. */
static lv_obj_t * s_arc_label;        /* CN：弧形中间的百分比。-- EN：Percentage label. */
static lv_obj_t * s_bar;              /* CN：被滑块控制的进度条。-- EN：Bar driven by the slider. */
static lv_obj_t * s_chart;            /* CN：折线图。-- EN：Chart. */
static lv_chart_series_t * s_series;  /* CN：折线图数据序列。-- EN：Chart data series. */
static lv_obj_t * s_btn_label;        /* CN：按钮文字。-- EN：Button label. */

static int32_t s_arc_value;           /* CN：弧形当前值。-- EN：Current arc value. */
static int32_t s_click_count;         /* CN：按钮点击次数。-- EN：Click counter. */

/**
 * @brief CN：周期定时器：推进弧形、给折线图加一个随机点。
 *        -- EN：Periodic timer: advance the arc and push a chart sample.
 */
static void anim_timer_cb( lv_timer_t * timer )
{
    LV_UNUSED( timer );

    s_arc_value += 2;
    if( s_arc_value > 100 ) {
        s_arc_value = 0;
    }

    lv_arc_set_value( s_arc, s_arc_value );
    lv_label_set_text_fmt( s_arc_label, "%d%%", ( int ) s_arc_value );
    lv_obj_center( s_arc_label );

    lv_chart_set_next_value( s_chart, s_series, rand() % 100 );
}

/**
 * @brief CN：滑块回调：把滑块的值同步给进度条。-- EN：Slider callback.
 */
static void slider_event_cb( lv_event_t * e )
{
    lv_obj_t * slider = lv_event_get_target( e );
    lv_bar_set_value( s_bar, lv_slider_get_value( slider ), LV_ANIM_ON );
}

/**
 * @brief CN：按钮回调：累加点击次数并刷新文字。-- EN：Button callback.
 */
static void button_event_cb( lv_event_t * e )
{
    LV_UNUSED( e );

    s_click_count++;
    lv_label_set_text_fmt( s_btn_label, "点击次数：%d", ( int ) s_click_count );
}

/**
 * @brief CN：搭建"控件演示"页。-- EN：Build the widget demo page.
 */
static void build_widgets_tab( lv_obj_t * tab )
{
    lv_obj_t * title = lv_label_create( tab );
    lv_label_set_text( title, "LVGL 9 / Windows Win32 后端" );
    lv_obj_set_style_text_font( title, &lv_font_cn_16, 0 );
    lv_obj_align( title, LV_ALIGN_TOP_MID, 0, 8 );

    /* 左侧：弧形进度条 */
    s_arc = lv_arc_create( tab );
    lv_obj_set_size( s_arc, 150, 150 );
    lv_arc_set_range( s_arc, 0, 100 );
    lv_arc_set_bg_angles( s_arc, 135, 45 );
    lv_arc_set_value( s_arc, 0 );
    lv_obj_align( s_arc, LV_ALIGN_TOP_LEFT, 30, 70 );

    s_arc_label = lv_label_create( s_arc );
    lv_label_set_text( s_arc_label, "0%" );
    lv_obj_center( s_arc_label );

    /* 中间：滑块 + 进度条 */
    lv_obj_t * slider = lv_slider_create( tab );
    lv_obj_set_width( slider, 210 );
    lv_slider_set_range( slider, 0, 100 );
    lv_slider_set_value( slider, 40, LV_ANIM_OFF );
    lv_obj_align( slider, LV_ALIGN_TOP_LEFT, 225, 95 );
    lv_obj_add_event_cb( slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL );

    s_bar = lv_bar_create( tab );
    lv_obj_set_size( s_bar, 210, 18 );
    lv_bar_set_value( s_bar, 40, LV_ANIM_OFF );
    lv_obj_align( s_bar, LV_ALIGN_TOP_LEFT, 225, 150 );
    lv_obj_set_style_bg_color( s_bar, lv_color_hex( 0x2F81F7 ), LV_PART_INDICATOR );

    /* 右侧：折线图 */
    s_chart = lv_chart_create( tab );
    lv_obj_set_size( s_chart, 300, 170 );
    lv_obj_align( s_chart, LV_ALIGN_TOP_LEFT, 470, 70 );
    lv_chart_set_type( s_chart, LV_CHART_TYPE_LINE );
    lv_chart_set_point_count( s_chart, 30 );
    lv_chart_set_range( s_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100 );
    s_series = lv_chart_add_series( s_chart, lv_color_hex( 0xF778BA ), LV_CHART_AXIS_PRIMARY_Y );

    /* 底部：按钮 */
    lv_obj_t * btn = lv_button_create( tab );
    lv_obj_set_size( btn, 160, 46 );
    lv_obj_align( btn, LV_ALIGN_BOTTOM_LEFT, 30, -20 );
    lv_obj_add_event_cb( btn, button_event_cb, LV_EVENT_CLICKED, NULL );

    s_btn_label = lv_label_create( btn );
    lv_label_set_text( s_btn_label, "点我" );
    lv_obj_center( s_btn_label );

    lv_obj_t * hint = lv_label_create( tab );
    lv_label_set_text( hint, "自己写页面：改 User/main.c，然后 make user" );
    lv_obj_set_style_text_font( hint, &lv_font_cn_16, 0 );
    lv_obj_align( hint, LV_ALIGN_BOTTOM_LEFT, 220, -30 );

    lv_timer_create( anim_timer_cb, 100, NULL );
}

#endif /* APP_UI_DEMO_WIDGETS */

/*============================================================================
 *  演示页 2：FreeRTOS 监视（APP_UI_RTOS_MONITOR）
 *============================================================================*/
#if APP_UI_RTOS_MONITOR

static lv_obj_t * s_rtos_label[ 6 ];  /* CN：六行统计文字。-- EN：Six statistics lines. */
static lv_obj_t * s_queue_bar;        /* CN：队列占用率。-- EN：Queue occupancy bar. */
static lv_obj_t * s_task_table;       /* CN：任务列表。-- EN：Task table. */

/**
 * @brief CN：每秒刷新几次监视页：读统计快照，更新文字、进度条和任务表。
 *        -- EN：Refresh the monitor page from a statistics snapshot.
 */
static void rtos_refresh_cb( lv_timer_t * timer )
{
    rtos_stats_t stats;
    rtos_task_info_t tasks[ 12 ];
    uint32_t task_count;
    char buf[ 8 ];
    char state[ 2 ];

    LV_UNUSED( timer );

    FreeRTOS_GetStats( &stats );

    lv_label_set_text_fmt( s_rtos_label[ 0 ], "心跳：%u", ( unsigned ) stats.heartbeat );
    lv_label_set_text_fmt( s_rtos_label[ 1 ], "队列 %u/%u   发 %u  收 %u  丢 %u",
                           ( unsigned ) stats.queue_depth, ( unsigned ) stats.queue_size,
                           ( unsigned ) stats.produced, ( unsigned ) stats.consumed,
                           ( unsigned ) stats.dropped );
    lv_label_set_text_fmt( s_rtos_label[ 2 ], "软件定时器：%u 次", ( unsigned ) stats.timer_ticks );
    lv_label_set_text_fmt( s_rtos_label[ 3 ], "信号量：成功 %u  超时 %u",
                           ( unsigned ) stats.sem_taken, ( unsigned ) stats.sem_timeout );
    lv_label_set_text_fmt( s_rtos_label[ 4 ], "互斥 %u  /  竞争 %u",
                           ( unsigned ) stats.lock_ok, ( unsigned ) stats.lock_bad );
    lv_label_set_text_fmt( s_rtos_label[ 5 ], "Tick %u ms   堆 %u B   最新值 %d",
                           ( unsigned ) stats.tick_ms, ( unsigned ) stats.heap_free,
                           ( int ) stats.last_value );

    if( stats.queue_size > 0u ) {
        lv_bar_set_value( s_queue_bar,
                          ( int32_t ) ( ( stats.queue_depth * 100u ) / stats.queue_size ),
                          LV_ANIM_OFF );
    }

    task_count = FreeRTOS_GetTaskList( tasks, 12 );
    lv_table_set_row_count( s_task_table, task_count + 1u );

    for( uint32_t i = 0; i < task_count; i++ ) {
        lv_table_set_cell_value( s_task_table, i + 1u, 0, tasks[ i ].name );

        state[ 0 ] = tasks[ i ].state;
        state[ 1 ] = '\0';
        lv_table_set_cell_value( s_task_table, i + 1u, 1, state );

        lv_snprintf( buf, sizeof( buf ), "%u", tasks[ i ].priority );
        lv_table_set_cell_value( s_task_table, i + 1u, 2, buf );

        lv_snprintf( buf, sizeof( buf ), "%u", tasks[ i ].stack_free );
        lv_table_set_cell_value( s_task_table, i + 1u, 3, buf );
    }
}

/**
 * @brief CN：搭建"FreeRTOS 监视"页。-- EN：Build the FreeRTOS monitor page.
 */
static void build_rtos_tab( lv_obj_t * tab )
{
    lv_obj_t * title = lv_label_create( tab );
    lv_label_set_text( title, "FreeRTOS V11.3.1（Windows 模拟）" );
    lv_obj_set_style_text_font( title, &lv_font_cn_16, 0 );
    lv_obj_align( title, LV_ALIGN_TOP_LEFT, 15, 4 );

    lv_obj_t * legend = lv_label_create( tab );
    lv_label_set_text( legend, "状态：R 运行   r 就绪   B 阻塞   S 挂起" );
    lv_obj_set_style_text_font( legend, &lv_font_cn_16, 0 );
    lv_obj_align( legend, LV_ALIGN_TOP_LEFT, 15, 26 );

    for( int i = 0; i < 6; i++ ) {
        s_rtos_label[ i ] = lv_label_create( tab );
        lv_label_set_text( s_rtos_label[ i ], "-" );
        lv_obj_align( s_rtos_label[ i ], LV_ALIGN_TOP_LEFT, 15, 50 + i * 22 );
    }

    lv_obj_t * bar_title = lv_label_create( tab );
    lv_label_set_text( bar_title, "队列占用率" );
    lv_obj_set_style_text_font( bar_title, &lv_font_cn_16, 0 );
    lv_obj_align( bar_title, LV_ALIGN_TOP_LEFT, 15, 186 );

    s_queue_bar = lv_bar_create( tab );
    lv_obj_set_size( s_queue_bar, 320, 14 );
    lv_obj_align( s_queue_bar, LV_ALIGN_TOP_LEFT, 15, 206 );
    lv_bar_set_range( s_queue_bar, 0, 100 );
    lv_obj_set_style_bg_color( s_queue_bar, lv_color_hex( 0x3FB950 ), LV_PART_INDICATOR );

    lv_obj_t * hint = lv_label_create( tab );
    lv_label_set_text( hint,
                       "mutex_a/b：每轮 +1，结果准确。\n"
                       "race_a/b：同样操作但不加锁，\n"
                       "部分累加会丢失。" );
    lv_obj_set_style_text_font( hint, &lv_font_cn_16, 0 );
    lv_obj_align( hint, LV_ALIGN_TOP_LEFT, 15, 236 );

    s_task_table = lv_table_create( tab );
    lv_obj_set_size( s_task_table, 420, 418 );
    lv_obj_align( s_task_table, LV_ALIGN_TOP_LEFT, 360, 4 );
    lv_obj_set_style_text_font( s_task_table, &lv_font_cn_16, LV_PART_ITEMS );
    lv_obj_set_style_pad_top( s_task_table, 2, LV_PART_ITEMS );
    lv_obj_set_style_pad_bottom( s_task_table, 2, LV_PART_ITEMS );
    lv_obj_set_style_pad_left( s_task_table, 4, LV_PART_ITEMS );
    lv_table_set_column_count( s_task_table, 4 );
    lv_table_set_column_width( s_task_table, 0, 110 );
    lv_table_set_column_width( s_task_table, 1, 70 );
    lv_table_set_column_width( s_task_table, 2, 85 );
    lv_table_set_column_width( s_task_table, 3, 70 );
    lv_table_set_cell_value( s_task_table, 0, 0, "任务" );
    lv_table_set_cell_value( s_task_table, 0, 1, "状态" );
    lv_table_set_cell_value( s_task_table, 0, 2, "优先级" );
    lv_table_set_cell_value( s_task_table, 0, 3, "栈" );

    lv_timer_create( rtos_refresh_cb, 300, NULL );
}

#endif /* APP_UI_RTOS_MONITOR */

/**********************
 *   STATIC FUNCTIONS（框架）
 **********************/

/**
 * @brief CN：统一每个标签页的深色外观。-- EN：Apply the shared dark look.
 */
static void style_tab( lv_obj_t * tab )
{
    /* CN：tab 容器默认透明，bg_opa 必须设成 COVER 背景色才显示得出来。
     * EN：Tab containers are transparent by default; bg_opa must be COVER. */
    lv_obj_set_style_bg_opa( tab, LV_OPA_COVER, 0 );
    lv_obj_set_style_bg_color( tab, lv_color_hex( 0x0E1620 ), 0 );
    lv_obj_set_style_text_color( tab, lv_color_hex( 0xE6EDF3 ), 0 );
    lv_obj_set_style_pad_all( tab, 0, 0 );
}

/**
 * @brief CN：按开关搭标签页。-- EN：Build tabs according to the switches.
 */
static void build_ui( void )
{
    lv_obj_t * screen = lv_screen_active();

    /* CN：屏幕默认字体换成中文字体，子控件自动继承。-- EN：CJK font for the screen. */
    lv_obj_set_style_text_font( screen, &lv_font_cn_16, 0 );

    lv_obj_t * tabview = lv_tabview_create( screen );
    lv_obj_set_size( tabview, WINDOW_W, WINDOW_H );

    int tab_count = 0;

#if APP_UI_DEMO_WIDGETS
    {
        lv_obj_t * tab = lv_tabview_add_tab( tabview, "控件演示" );
        style_tab( tab );
        build_widgets_tab( tab );
        tab_count++;
    }
#endif

#if APP_UI_RTOS_MONITOR
    {
        lv_obj_t * tab = lv_tabview_add_tab( tabview, "FreeRTOS" );
        style_tab( tab );
        build_rtos_tab( tab );
        tab_count++;
    }
#endif

    if( tab_count == 0 ) {
        /* CN：开关全关时的兜底。-- EN：Fallback when every switch is 0. */
        style_tab( lv_tabview_add_tab( tabview, "空" ) );
        tab_count = 1;
    }

    if( APP_UI_START_TAB < tab_count ) {
        lv_tabview_set_active( tabview, APP_UI_START_TAB, LV_ANIM_OFF );
    }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void Lvgl_App_InitWindow( void )
{
    if( s_window_inited ) {
        return;
    }
    s_window_inited = true;

    setvbuf( stdout, NULL, _IONBF, 0 );  /* CN：日志立刻可见。-- EN：Unbuffered stdout. */

    lv_init();  /* CN：LV_USE_OS == LV_OS_WINDOWS 时会顺带初始化 Win32 后端。-- EN：Init core. */

    lv_display_t * display = lv_windows_create_display(
                                 L"DeskBoard - LVGL 9 + FreeRTOS",
                                 WINDOW_W,
                                 WINDOW_H,
                                 100,
                                 false,
                                 false );

    if( display == NULL ) {
        printf( "[lvgl] create display failed\n" );
        return;
    }

    /* CN：主题字体换成中文字体（抽成函数，自己建窗口时也能直接调）。
     * EN：Theme/screen font to the CJK font (also usable when you create the
     * display yourself). */
    Lvgl_App_UseChineseFont( display );

#if defined( _WIN32 )
    /* CN：把窗口提到最前，双击 exe 时不用去任务栏找。-- EN：Bring the window to front. */
    {
        HWND window = lv_windows_get_display_window_handle( display );
        if( window != NULL ) {
            SetWindowPos( window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
            SetWindowPos( window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
            SetForegroundWindow( window );
        }
    }
#endif

    printf( "[lvgl] window is running\n" );
}

void Lvgl_App_UseChineseFont( lv_display_t * display )
{
    if( display == NULL ) {
        return;
    }

    /* CN：主题字体 = 中文字体，主题创建的按钮/标签/表格默认就能显示中文。
     * EN：Theme font = CJK font so themed widgets render Chinese by default. */
    lv_theme_t * theme = lv_theme_default_init( display,
                                                lv_palette_main( LV_PALETTE_BLUE ),
                                                lv_palette_main( LV_PALETTE_RED ),
                                                false,
                                                &lv_font_cn_16 );
    if( theme != NULL ) {
        lv_display_set_theme( display, theme );
    }

    /* CN：屏幕默认字体也换掉，自己 new 出来的控件（不经过主题）同样继承。
     * EN：Also set the screen font so hand-made objects inherit it. */
    lv_obj_set_style_text_font( lv_screen_active(), &lv_font_cn_16, 0 );
}

void Lvgl_App_Init( void )
{
    Lvgl_App_InitWindow();

    if( s_window_inited && !s_ui_built ) {
        s_ui_built = true;
        build_ui();
    }
}

bool Lvgl_App_Step( uint32_t * delay_ms )
{
    uint32_t time_till_next = lv_timer_handler();

    if( ( time_till_next == LV_NO_TIMER_READY ) || ( time_till_next > 20u ) ) {
        time_till_next = 20u;
    }
    if( time_till_next == 0u ) {
        time_till_next = 1u;
    }

    if( delay_ms != NULL ) {
        *delay_ms = time_till_next;
    }
    return true;
}

void Lvgl_App_Run( void )
{
    uint32_t delay_ms = 5;

    /* CN：这里不初始化——调用前先 Lvgl_App_InitWindow() 或 Lvgl_App_Init()。
     * EN：No init here; call Lvgl_App_InitWindow()/Lvgl_App_Init() first. */
    for( ;; ) {
        ( void ) Lvgl_App_Step( &delay_ms );
        lv_delay_ms( delay_ms );
    }
}
