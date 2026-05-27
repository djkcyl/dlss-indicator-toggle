# DLSS Indicator Toggle

NVIDIA DLSS 有个隐藏的调试覆盖层，开启后游戏里能实时看到 DLSS DLL 版本、渲染分辨率、Preset、DLSS-G 帧率倍率、Reflex 状态等。这个工具就是切换它的开关——本质上改一个注册表 DWORD：`HKLM\SOFTWARE\NVIDIA Corporation\Global\NGXCore\ShowDlssIndicator`。

| 值 | 效果 |
|---|---|
| `0` / 未设置 | 关闭 |
| `1` | 绿色方块（仅 dev DLL） |
| `1024` | 文字详情（SR/RR 左下角 + DLSS-G 顶部） |

不想用工具的话直接进 regedit 改这个值也是一样的，这只是省得手动建 key。136 KB 单 exe，零依赖。

## 用法

从 [Releases](../../releases) 下载 exe，双击运行（需管理员权限），改完重启游戏。

## 编译

需要 VS 2022 + C++ 工作负载。`_build_msvc.bat` 一键编译。

## License

MIT
