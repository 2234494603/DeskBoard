# ============================================================================
#  C_language_practice_project  ——  Windows + gcc (MSYS2 UCRT64) 构建脚本
#
#  make           生成 DeskBoard.exe（系统演示：System/System_main.c）
#  make user      编译 User/main.c 并直接运行（新同学用这个）
#  make run       生成系统演示后直接运行
#  make clean     删除 build/ 和两个 exe
#  make -j8       多线程编译（LVGL 有 480 个 .c，建议加 -j）
# ============================================================================

CC      := gcc
BUILD   := build

# ---------------- 目录布局（User / System 两层） ----------------
SYS_DIR      := System
APP_INC      := $(SYS_DIR)/App/inc
APP_SRC      := $(SYS_DIR)/App/src
DS_DIR       := $(SYS_DIR)/Drive/DataStructures
LVGL_DIR     := $(SYS_DIR)/Drive/LVGL_GUI
FREERTOS_DIR := $(SYS_DIR)/Drive/FreeRTOS

# ---------------- 入口选择 ----------------
# ENTRY=system（默认）跑系统演示；
#ENTRY=user 编译 User/main.c
# 两个入口各生成一个 exe，互不覆盖，切换时不用先 clean。
USER_DIR := User
ENTRY ?= system
ifeq ($(ENTRY),user)
  ENTRY_SRC  := $(USER_DIR)/main.c
  # User/ 下自己新增的 .c 全部自动参与编译（main.c 已经在 ENTRY_SRC 里，去掉避免重复）
  USER_EXTRA := $(filter-out $(ENTRY_SRC),$(wildcard $(USER_DIR)/*.c))
  TARGET     := DeskBoard_user
else
  ENTRY_SRC  := $(SYS_DIR)/System_main.c
  USER_EXTRA :=
  TARGET     := DeskBoard
endif

# ----------------------------------------------------------------------------
# 1. 收集源文件
# ----------------------------------------------------------------------------

# 框架 + 练习模块：只取自己写的目录，绝不递归到 LVGL / FreeRTOS
APP_SRCS := $(wildcard $(APP_SRC)/*.c)
DS_SRCS  := $(wildcard $(DS_DIR)/src/*.c)

# LVGL 的 .c 分散在 src 下面几十个子目录里，用 GNU Make 的递归通配符收集。
# 不用 $(shell find ...)：Windows 上 $(shell) 走 cmd.exe，find 会被解析成
# C:\Windows\System32\find.exe（DOS 的字符串查找工具），直接报错。
rwildcard  = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
LVGL_SRCS := $(call rwildcard,$(LVGL_DIR)/src,*.c)

# FreeRTOS：内核核心 + Windows 模拟端口 + heap_4
RTOS_SRCS := $(wildcard $(FREERTOS_DIR)/src/*.c) \
             $(FREERTOS_DIR)/portable/MSVC-MingW/port.c \
             $(FREERTOS_DIR)/portable/MemMang/heap_4.c

SRCS := $(ENTRY_SRC) $(USER_EXTRA) $(APP_SRCS) $(DS_SRCS) $(LVGL_SRCS) $(RTOS_SRCS)
OBJS := $(patsubst %.c,$(BUILD)/%.o,$(SRCS))

# ----------------------------------------------------------------------------
# 2. 编译选项
# ----------------------------------------------------------------------------

# 头文件搜索路径：App/inc 放框架接口，DataStructures/inc 放练习模块，
# User 放你自己的头文件（这样 #include "led.h" 就能找到 User/led.h）
INCLUDES := -I$(APP_INC) -I$(DS_DIR)/inc -I$(USER_DIR) -I$(LVGL_DIR) -I$(LVGL_DIR)/src

# FreeRTOS 头文件：内核、Windows 端口、FreeRTOSConfig.h
RTOS_INCLUDES := -I$(FREERTOS_DIR)/include \
                 -I$(FREERTOS_DIR)/portable/MSVC-MingW \
                 -I$(FREERTOS_DIR)

# 自己的代码（含 User/main.c）：UTF-8，中文界面与控制台都不会乱码
USER_CFLAGS := -std=gnu11 -Wall -g $(INCLUDES) $(RTOS_INCLUDES) \
               -finput-charset=UTF-8 -fexec-charset=UTF-8

# LVGL 源码
LVGL_CFLAGS := -std=gnu11 -w -O2 $(INCLUDES) \
               -finput-charset=UTF-8 -fexec-charset=UTF-8

# FreeRTOS 源码
RTOS_CFLAGS := -std=gnu11 -w -O2 $(RTOS_INCLUDES) \
               -finput-charset=UTF-8 -fexec-charset=UTF-8

# 按目标所在目录挑编译选项
CFLAGS_OF = $(if $(filter $(LVGL_DIR)/%,$<),$(LVGL_CFLAGS),\
              $(if $(filter $(FREERTOS_DIR)/%,$<),$(RTOS_CFLAGS),$(USER_CFLAGS)))

# -MMD -MP：生成头文件依赖，改 System_config.h 会自动重编受影响的文件
DEPFLAGS := -MMD -MP
DEPS     := $(OBJS:.o=.d)

# 链接库：gdi32 画像素、user32 窗口消息、imm32 输入法、winmm 提高定时器精度
LDLIBS := -lgdi32 -luser32 -limm32 -lwinmm -static

# ----------------------------------------------------------------------------
# 3. Windows / POSIX 差异
# ----------------------------------------------------------------------------

ifeq ($(OS),Windows_NT)
  EXE     := .exe
  winpath  = $(subst /,\,$(patsubst %/,%,$(1)))
  MKDIR    = if not exist "$(call winpath,$(1))" mkdir "$(call winpath,$(1))"
  RMDIR    = if exist "$(call winpath,$(1))" rmdir /S /Q "$(call winpath,$(1))"
  RMFILE   = if exist "$(call winpath,$(1))" del /Q "$(call winpath,$(1))"
  # CN：Makefile 本身是 UTF-8，而 Windows 控制台默认是 GBK(936)，直接 echo 中文会乱码；
  #     先把当前控制台切到 UTF-8(65001)，下面 user 目标里的中文提示才能正常显示。
  # EN：This Makefile is UTF-8 but the Windows console defaults to GBK(936), so Chinese
  #     echoed directly comes out garbled; switch the console to UTF-8(65001) first.
  CHCP_UTF8 = chcp 65001 >nul
else
  EXE     :=
  MKDIR    = mkdir -p "$(1)"
  RMDIR    = rm -rf "$(1)"
  RMFILE   = rm -f "$(1)"
  CHCP_UTF8 = true
endif

# ----------------------------------------------------------------------------
# 4. 规则
# ----------------------------------------------------------------------------

.PHONY: all run clean user

all: $(TARGET)$(EXE)

$(TARGET)$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDLIBS)

$(BUILD)/%.o: %.c
	@$(call MKDIR,$(dir $@))
	$(CC) $(CFLAGS_OF) $(DEPFLAGS) -c $< -o $@

-include $(DEPS)

run: $(TARGET)$(EXE)
	$(TARGET)$(EXE)

# 新同学看这里：改完 User/main.c 后，在项目根目录输入
#     make user
# 就会自动编译并弹出窗口（窗口关掉后这条命令才会结束）。
user:
	@$(CHCP_UTF8)
	@$(MAKE) ENTRY=user --no-print-directory all
	@echo.
	@echo 正在运行你的程序，关掉窗口后本命令结束。
	@DeskBoard_user$(EXE)

clean:
	@$(call RMDIR,$(BUILD))
	@$(call RMFILE,$(TARGET)$(EXE))
	@$(call RMFILE,DeskBoard.exe)
	@$(call RMFILE,DeskBoard_user.exe)
	@echo clean done
