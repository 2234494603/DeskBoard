# 中文字体说明

LVGL 自带的 Montserrat 只有拉丁字母，显示中文会变成方块；它自带的思源黑体子集又只有
1195 个字（`演`、`页`、`端` 这些常用字都缺）。所以本工程用 **lv_font_conv** 把
`Drive/TTF/dingdingjinbuti.ttf`（钉钉进步体）转成了 LVGL 能直接编译的字体源码
`App/font_cn_16.c`。

## 已经生成好的字体

| 项目 | 值 |
| --- | --- |
| 源字体 | `Drive/TTF/dingdingjinbuti.ttf` |
| 字号 / 位深 | 16 px / 4 bpp |
| 字符集 | ASCII + GB2312 全部（6763 汉字 + 标点符号），共 8273 个字符 |
| 生成文件 | `App/font_cn_16.c`（约 5.7 MB，编译后约 700 KB） |
| 字体符号 | `lv_font_cn_16`，在 `App/app_config.h` 里用 `LV_FONT_DECLARE` 声明 |

界面里所有文字都继承它：`Lvgl_App_Init()` 里把主题字体和屏幕默认字体都换成了
`lv_font_cn_16`，所以你新加的控件不用单独设字体就能显示中文。

## 想换字体 / 换字号怎么重新生成

1. 把新的 `.ttf` / `.otf` 放进 `Drive/TTF/`。
2. 安装一次转换工具（只需一次）：

   ```powershell
   npm install --prefix build\.lvfont lv_font_conv
   ```

3. 生成字体（把路径和字号换成自己的）：

   ```powershell
   node build\.lvfont\node_modules\lv_font_conv\lv_font_conv.js `
     -o App\font_cn_16.c --no-compress `
     --font Drive\TTF\dingdingjinbuti.ttf `
     --size 16 --bpp 4 --format lvgl --lv-include lvgl.h `
     --lv-font-name lv_font_cn_16 `
     --symbols "<字符集字符串>"
   ```

   `--symbols` 里放要包含的字符。本项目用的是"ASCII + GB2312 全部"，可以用下面的
   PowerShell 片段生成这个字符串：

   ```powershell
   $enc = [Text.Encoding]::GetEncoding(936)
   $sb = New-Object Text.StringBuilder
   for ($c = 0x20; $c -le 0x7E; $c++) { [void]$sb.Append([char]$c) }
   for ($b1 = 0xA1; $b1 -le 0xF7; $b1++) {
     for ($b2 = 0xA1; $b2 -le 0xFE; $b2++) {
       $s = $enc.GetString([byte[]]@($b1, $b2))
       if ($s.Length -eq 1 -and [int][char]$s -ne 0xFFFD) { [void]$sb.Append($s) }
     }
   }
   $symbols = $sb.ToString()
   ```

4. `make -j8` 重新编译即可。

> 注意两点：
> - **必须加 `--no-compress`**。压缩格式的 `bitmap_format` 与当前 LVGL 9 对不上，
>   文字会直接不显示。
> - 改了 `--lv-font-name` 的话，`App/app_config.h` 里的 `LV_FONT_DECLARE` 和
>   `lvgl_app.c` 里的字体引用要一起改。

## 字体许可

钉钉进步体（DingTalk JinBuTi）由钉钉免费提供，允许个人与企业免费使用（含商用）。
如果换成其它字体，请自行确认许可，并把许可文件一并放进 `Drive/TTF/`。
