# DLSS Indicator Toggle

136 KB 的 Win32 工具，切换 `HKLM\SOFTWARE\NVIDIA Corporation\Global\NGXCore\ShowDlssIndicator` 注册表值。

| 值 | 效果 |
|---|---|
| `0` / 未设置 | 关闭 |
| `1` | 绿色方块（仅 dev DLL） |
| `1024` | 文字详情（SR/RR 左下角 + DLSS-G 顶部） |

## 用法

从 [Releases](../../releases) 下载 exe，双击运行（需管理员权限），改完重启游戏。

## 编译

需要 VS 2022 + C++ 工作负载。`_build_msvc.bat` 一键编译。

## License

MIT
