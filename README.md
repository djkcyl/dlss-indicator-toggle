# DLSS Indicator Toggle

一个约 140 KB 的 Win32 原生小工具，用来切换 NVIDIA DLSS 屏幕指示器的注册表开关。零运行时依赖，双击即用。

## 这是什么

NVIDIA DLSS 有一个隐藏的调试覆盖层，开启后能在游戏里实时看到：

- DLSS DLL 版本与路径
- 渲染分辨率 → 输出分辨率
- 当前 Preset（A/B/C/D/E/F/J/K…）
- DLSS-G 帧生成的真实/输出帧率与生成倍率
- Reflex 状态

控制开关的注册表值是 `HKLM\SOFTWARE\NVIDIA Corporation\Global\NGXCore\ShowDlssIndicator` (DWORD)：

| 值 | 效果 |
|---|---|
| 未设置 / `0` | 关闭 |
| `1` | 屏幕角落绿色小方块（仅证明 DLSS 在跑） |
| `1024` (`0x400`) | 文字详情，同时点亮 SR/RR 左下角与 DLSS-G 顶部覆盖层 |

这个工具让你不用进 regedit 就能改这个值。

## 怎么用

1. 从 [Releases](../../releases) 下载 `DlssIndicatorToggle.exe`
2. 双击运行 — 会请求管理员权限（因为要写 `HKEY_LOCAL_MACHINE`）
3. 勾选"应用此项"，选择目标值，点击"应用"
4. 重启游戏生效

UI 行为说明：

- **复选框默认状态跟着注册表走** — key 不存在时默认不勾选（保持现状），存在时默认勾选。不勾选点应用 = 跳过此项不写入
- **"在 regedit 中打开"按钮** — 设置 regedit 的 `LastKey` 让它自动跳转到 NGXCore，同时把完整路径复制到剪贴板兜底（如果未跳转，地址栏 Ctrl+V 即可）

## 自己编译

需要 **Visual Studio 2022**（社区版即可，含 "Desktop development with C++" 工作负载）。

```bat
_build_msvc.bat
```

`_build_msvc.bat` 会先调 `vcvars64.bat` 设置开发环境，再 `cd` 回项目目录调用 `build.bat`。如果你已经在 Developer Command Prompt 里，直接：

```bat
build.bat
```

产物：`DlssIndicatorToggle.exe`，约 140 KB，链接的都是 Windows 系统自带 DLL（user32 / advapi32 / comctl32 / shell32 / ole32），无任何第三方运行时依赖。Win 7 及以上应该都能跑。

`build.bat` 也支持 MinGW（在 PATH 里有 `g++` 时会自动用），但用 MSVC 产出体积更小。

## 文件结构

```
main.cpp           # 全部代码，约 200 行
resource.rc        # 对话框模板 + 嵌入 manifest
resource.h         # 控件 ID 定义
app.manifest       # UAC 提权（requireAdministrator）+ 启用 ComCtl32 v6 主题 + PerMonitorV2 DPI
build.bat          # 编译脚本，自动选择 MSVC 或 MinGW
_build_msvc.bat    # MSVC 包装脚本（设置 vcvars 后调 build.bat）
```

## 关于 DLSS-G、EnableOTA

开发过程中我曾经以为 `ShowDlssGIndicator` 和 `EnableOTA` 是独立的注册表值——查证后发现**这两个都不存在**：

- DLSS-G 的顶部覆盖层和 SR/RR 的左下角共用 `ShowDlssIndicator`，`1024` 同时点亮两者
- `EnableOTA` 在 NVIDIA 官方资料与社区资料里都查不到

如果你想阻止 NVIDIA 通过 OTA 覆盖你手动换的 DLL，正确做法是用 [NVIDIA Profile Inspector](https://github.com/Orbmu2k/nvidiaProfileInspector) 配置 "DLSS Override" 让游戏走驱动自带 DLL，而不是改注册表。

## 参考

- NVIDIA 官方 .reg 模板（值 `1` 版本）：[NVIDIA/DLSS — ngx_driver_onscreenindicator.reg](https://github.com/NVIDIA/DLSS/blob/main/utils/ngx_driver_onscreenindicator.reg)
- 社区 .reg 模板（值 `1024` 版本，同时覆盖 dev/prod DLL）：[emoose/DLSSTweaks](https://github.com/emoose/DLSSTweaks/blob/master/external/ngx_driver_onscreenindicator_all.reg)
- 注册表字段列表：[DLSSTweaks issue #139](https://github.com/emoose/DLSSTweaks/issues/139)
- 中文介绍：[PC Gamer — This tiny registry tweak shows what resolution DLSS is rendering at](https://www.pcgamer.com/nvidia-dlss-indicator/)

## License

MIT
