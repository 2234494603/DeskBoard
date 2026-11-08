# Git 提交规则 · DeskBoard

> 本文件是本仓库的落地版规则，配套 `.gitignore` 使用。
> 适用目录：`C:\Users\Administrator\Documents\Adobe\C_language_practice_project`
> 更新时间：2026-11-08

---

## 1. 仓库现状

| 项目 | 情况 |
| --- | --- |
| 已跟踪文件 | 3121 个，约 58.7 MB |
| 构建方式 | `make -j8` → `DeskBoard.exe`（系统演示）；`make user -j8` → `DeskBoard_user.exe`（User/main.c）。gcc + GNU Make，Windows |
| 现有提交风格 | 纯中文、无类型前缀（如 `新增FreeRtos+LVGL来练习API`） |

目录职责：

| 路径 | 内容 | 处理 |
| --- | --- | --- |
| `User/main.c` | 你自己的入口：建窗口 → 搭页面 → 主循环 | 提交 |
| `User/*.c`、`User/*.h` | 你自己的模块（如 `led.c/.h`） | 提交 |
| `System/System_main.c` | 系统演示入口（`make` 用） | 提交 |
| `System/App/inc/`、`System/App/src/` | 框架头文件与实现（`System_config.h`、`lvgl_app.c`、`freertos_app.c`、`font_cn_16.c`） | 提交 |
| `System/Drive/DataStructures/` | `Calc.c`、`List.c` 练习模块（`inc/` + `src/`） | 提交 |
| `System/Drive/FreeRTOS/src/`、`include/`、`portable/`、`FreeRTOSConfig.h` | FreeRTOS V11.3.1 内核 + Windows 模拟端口（**参与构建**） | 提交 |
| `System/Drive/LVGL_GUI/` | **唯一**的 LVGL 源码（`src/` + `include/` + `demos/`）+ `lv_conf.h` | 提交（见 §4.2） |
| `System/Drive/TTF/` | 原始字体文件（生成 `font_cn_16.c` 用） | 提交 |
| `System/docs/` | 截图、`font.md`、`System/docs/解释脚本.txt` | 提交 |
| `build/` | 编译中间产物 | **禁止提交** |
| `DeskBoard.exe`、`DeskBoard_user.exe` | 两个入口的交付产物，双击即可运行 | 提交（见 §4.1） |

---

## 2. 允许提交的文件

| 类别 | 扩展名 / 文件名 | 本仓库实例 |
| --- | --- | --- |
| 源码 | `*.c` `*.h` | `User/main.c`、`System/App/src/lvgl_app.c`、`System/Drive/LVGL_GUI/src/**` |
| 文档 | `*.md` `*.txt` `*.pdf` | `README.md`、`System/docs/解释脚本.txt`、本文件、`System/Drive/LVGL_GUI/LICENCE.txt` |
| 构建配置 | `Makefile` `*.mk` `*.bat` `*.json` `*.cbp` | `Makefile`、`System/Drive/LVGL_GUI/lvgl.mk` |
| 链接 / 启动 | `*.ld` `*.S` `*.s` | 目前无；引入时须说明用途 |
| 预编译库 | `*.a` | 目前无；引入时须是项目必需且来源可追溯 |
| 固件 / 交付 | `*.bin` `*.map` `*.exe` | `DeskBoard.exe`、`DeskBoard_user.exe` |
| 图片 | `*.png` `*.jpg` `*.gif` `*.svg` 等 | `System/docs/*.png`（目前暂无截图） |
| 字体 | `*.ttf` `*.otf` | `System/Drive/TTF/dingdingjinbuti.ttf`、`System/Drive/LVGL_GUI/src/font/**` |
| 仓库规则 | `.gitignore` `.gitattributes` | `.gitignore` |

> 有扩展名 ≠ 自动放行。提交前必须确认该文件**确实被项目使用**（源码、配置、资源、构建脚本或交付物）。

---

## 3. 必须忽略的文件

| 类别 | 规则 | 说明 |
| --- | --- | --- |
| 编译中间产物 | `/build/`、`*.o`、`*.obj`、`*.d`、`*.dep`、`*.elf`、`*.out` | 都能由 `make` 重新生成 |
| 本地环境 | `/.vscode/`、`*.code-workspace`、`Thumbs.db`、`desktop.ini` | 只属于某台电脑 |
| Python 缓存 | `__pycache__/`、`*.pyc` | FreeRTOS 工具链运行后产生 |
| 调试临时文件 | `/out.txt`、`/err.txt`、`/screenshot.png`、`/full.png`、`/shot*.png`、`*.log` | 验证过程产物，不是交付物 |
| 编辑器临时文件 | `*.swp`、`*~` | — |

以上规则已写入 `.gitignore`，并实测：`build/System/System_main.o`、`out.txt` 被忽略；`System/Drive/LVGL_GUI/lv_conf.h`、`System/App/src/lvgl_app.c`、`System/docs/font.md`、`DeskBoard.exe` 均未被误伤。

> **重要**：`.gitignore` 只对「未被跟踪」的文件生效。已被跟踪的文件即使命中规则也不会自动取消跟踪，必须用
> `git rm --cached <具体路径>` 处理（只删索引、不删本地文件），且**必须先经确认**。

---

## 4. 本仓库的特殊决策

### 4.1 `DeskBoard.exe` / `DeskBoard_user.exe`：保留跟踪

- 理由：两个 exe 都是交付产物（演示版 / 你自己的入口），不装编译器也能双击运行，符合"必要交付产物"。
- 约束：只在**确认可运行**的版本上提交，并在提交信息里说明；不要每编译一次就提交一次。
- 若以后改为发布制（打 tag 上传产物），再把它移出版本库。

### 4.2 LVGL 只有一份：`System/Drive/LVGL_GUI/`（已整合）

整合前的状态是两份副本（根目录 `LVGL_GUI/` 与 `System/Drive/LVGL_GUI/`），仓库体积翻倍且同名易混淆。
**2026-11-08 已整合完成**，现在全仓库只有 `System/Drive/LVGL_GUI/` 这一份：

| 内容 | 说明 |
| --- | --- |
| `System/Drive/LVGL_GUI/src/` | LVGL 源码（1005 个文件），与根目录那份逐字节一致 |
| `System/Drive/LVGL_GUI/include/` | LVGL 公共头文件（209 个文件），整合前旧副本缺失，导致无法编译 |
| `System/Drive/LVGL_GUI/demos/` | 官方 demo（含 `widgets`），默认不参与构建 |
| `System/Drive/LVGL_GUI/lv_conf.h` | **Windows 构建使用**：`LV_OS_WINDOWS` + `XRGB8888` |
| `System/Drive/LVGL_GUI/lv_conf_mcu.h` | 整合前那份 MCU 配置的备份：`LV_OS_NONE` + `RGB565`，不参与构建 |
| `System/Drive/LVGL_GUI/lvgl.h` 等顶层文件 | LVGL 入口头、模板配置、`lvgl.mk`、`LICENCE.txt` |

规则：

- `Makefile` 的 `LVGL_DIR` 固定为 `System/Drive/LVGL_GUI`，不得再新增第二份 LVGL 副本。
- 切换 MCU/Windows 配置时，用改名替换 `lv_conf.h`，不要复制整个源码目录。
- `System/Drive/LVGL_GUI/**` 的 `.c`、`.h` 必须提交，禁止用 `*.c` 一类的宽泛规则误伤。

### 4.3 `System/Drive/FreeRTOS/`：只保留构建必需的内核

- **参与构建**：`src/`（内核 7 个 .c）、`include/`（21 个头文件）、`portable/MSVC-MingW/`、
  `portable/MemMang/heap_4.c`、`FreeRTOSConfig.h`、`LICENSE.md`。这些是项目运行必需，必须提交。
- **已清理（2026-11-08）**：原先的 `System/Drive/FreeRTOS/port/` 是 FreeRTOS-Plus-TCP 协议栈
  及其上游测试工具链（CMock / CBMC / IAR / SEGGER 工程，2067 个文件、30.6 MB），
  不参与本项目构建，已整体删除；`include/` 下的 `CMakeLists.txt`、`stdint.readme`、
  `portable/MemMang/ReadMe.url` 一并删除。删除后目录从 32.1 MB 降到 1.47 MB。
- 如将来要做 TCP，需从 FreeRTOS-Plus-TCP 官方仓库重新拉取，不要恢复这批测试工具链。

### 4.4 `System/docs/` 图片

- `System/docs/lvgl_window.png` 是界面效果截图，属于文档资源，允许提交。
- 后续补充的截图也应放在 `System/docs/` 并对文件名做语义化命名（如 `lvgl_window.png`），不要散落在仓库根目录。

---

## 5. 提交信息规范

格式：

```
<type>(<scope>): <中文摘要>

<可选正文：为什么改、影响范围、验证方式>
```

`type` 取值：

| type | 用途 |
| --- | --- |
| `feat` | 新增功能 |
| `fix` | 修复缺陷 |
| `refactor` | 重构（行为不变） |
| `build` | 构建系统 / 依赖变更 |
| `docs` | 文档 |
| `chore` | 仓库维护（`.gitignore`、清理等） |
| `ci` | 持续集成配置 |

`scope` 建议：`calc`、`list`、`lvgl`、`build`、`docs`、`repo`。

要求：

1. 摘要用中文，≤ 50 字，动词开头，不加句号。
2. 一次提交只做一件事，不混入无关修改。
3. 禁止"更新文件""修改代码""提交"这类无信息量的描述。
4. 正文写清动机与验证方式（例如"`make -j8` 通过，窗口正常显示"）。

示例（本仓库）：

```text
feat(lvgl): 使用 Win32 后端在 Windows 上运行 LVGL 界面
build(make): 修复 Windows 下 find 与 mkdir 的兼容问题
fix(list): 移除 List.c 中重复的 struct List 定义
docs: 补充 LVGL 编译运行说明与界面截图
chore(repo): 新增 .gitignore 与仓库提交规则
```

反例：`更新`、`修改代码`、`提交一下`、`修复bug`。

> 历史提交为纯中文风格，新提交请使用"类型前缀 + 中文摘要"，不必改写历史。

---

## 6. 提交前检查清单

```powershell
git status --short                 # 1. 看工作区与暂存区
git diff                           # 2. 审查未暂存修改
git diff --cached                  # 3. 审查已暂存修改
git ls-files | Select-String '\.(o|obj|d|dep|elf|out)$'   # 4. 确认中间产物没进库（应为空）
git check-ignore -v <文件>          # 5. 新文件是否命中忽略规则
git ls-files --error-unmatch <文件> # 6. 该文件是否已被跟踪
git add <具体路径>                  # 7. 精确暂存，不用 git add .
```

人工确认四项：

- [ ] GUI Guider 生成的 `.c` / `.h` 没有被忽略（本仓库暂无该目录，规则已预留）。
- [ ] `System/Drive/LVGL_GUI/**` 的源码和手工维护的 `lv_conf.h` 都在待提交列表里。
- [ ] 没有与本次任务无关的改动（例如顺手格式化）。
- [ ] 临时文件（`out.txt`、`screenshot.png`、`build/`）没有混入。

> 小提示：`git status` 里 `System/docs/解释脚本.txt` 会显示成 `"\350\247\243..."`，这是 Git 对非 ASCII 文件名的转义。执行一次
> `git config core.quotepath false`（只改本地配置，不影响历史）即可正常显示中文。

---

## 7. 已知冲突与建议清理（均需授权后执行）

| # | 内容 | 数量 | 位置 | 冲突条款 | 建议 |
| --- | --- | --- | --- | --- | --- |
| 1 | 已跟踪的 `lv_conf.h` 被替换为 Windows 版 | 1 个文件 | `System/Drive/LVGL_GUI/lv_conf.h` | 配置需与构建一致 | 旧 MCU 配置已另存为 `lv_conf_mcu.h`，无需回滚 |
| 2 | 已跟踪的 `System/Drive/FreeRTOS/port/` 已整体删除 | 2067 个文件 | `System/Drive/FreeRTOS/` | 含 `.pyc`/`.dep`/IAR 工程等非白名单文件 | 本地已删；提交时用 `git add -A System/Drive/FreeRTOS` 让删除生效，历史仍可 `git restore` 找回 |

统一说明：

- `git rm -r --cached <路径>` **只取消跟踪，不删除本地文件**，但会让其他人 `pull` 后失去这些文件，属于影响仓库内容的操作，执行前必须获得你的明确授权。
- 在没有授权前，我不会删除、移动或改写任何已跟踪文件。

---

## 8. 安全红线

未经明确授权，禁止执行：

- `git commit`、`git push`
- `git reset --hard`、`git clean -fd`
- 强制推送、改写历史、删除远程分支/标签
- 批量删除本地文件、覆盖你已有的修改
- `git add -f` 强制添加被忽略的文件

未经要求，不执行编译（`make`）。

---

## 9. 规则维护

1. 修改 `.gitignore` 时，必须同步更新本文件第 3 节。
2. 新增第三方库时：源码原样提交，配置文件（如 `lv_conf.h`）单独维护并提交，不整目录打包上传。
3. 本文件与 `.gitignore` 冲突时，以本文件的人工判定为准，并修正 `.gitignore`。
