# Source Engine（起源引擎）
[![GitHub Actions Status](https://github.com/nillerusr/source-engine/actions/workflows/build.yml/badge.svg)](https://github.com/nillerusr/source-engine/actions/workflows/build.yml) [![GitHub Actions Status](https://github.com/nillerusr/source-engine/actions/workflows/tests.yml/badge.svg)](https://github.com/nillerusr/source-engine/actions/workflows/tests.yml)
 Discord: [![Discord Server](https://img.shields.io/discord/672055862608658432.svg)](https://discord.gg/hZRB7WMgGw)
 

来自 [wikipedia](https://wikipedia.org/wiki/Source_(game_engine)) 的信息：

Source（起源）是由 Valve 开发的 3D 游戏引擎。
它于 2004 年 6 月在《半条命：起源》(Half-Life: Source) 中作为 GoldSrc 的继任者首次亮相，
同年晚些时候又推出了《反恐精英：起源》(Counter-Strike: Source) 和《半条命 2》(Half-Life 2)。
Source 没有简洁的版本号体系，而是以增量版本的形式发布。

源代码基于 TF2 2018 年的泄露版本。请勿将其用于商业用途。

本项目使用 waf 构建系统。如果你有关于 waf 的问题，请参阅 https://waf.io/book

# 特性：
- 支持 Android、OSX、FreeBSD、Windows、Linux（glibc、musl）
- 支持 ARM（Windows 除外）
- 支持 64 位
- 支持现代工具链
- 修复了许多未定义行为（undefined behaviour）
- 支持触屏操作（即使在 Windows/Linux/OSX 上）
- 支持 VTF 7.5
- 支持 PBR（基于物理的渲染）
- 支持 bsp v19-v21（bsp v21 为部分支持，Portal 2 和 CS:GO 地图可正常运行）
- 支持 mdl v46-49
- 移除了无用/不必要的依赖
- 成就系统无需 Steam 即可工作
- 修复了许多 Bug
- 服务器浏览器无需 Steam 即可工作

# 当前任务
- 为 OpenGL 渲染重写 materialsystem（材质系统）
- 支持 dxvk-native
- Elbrus 移植
- Bink 音频支持（用于 video_bink）

# 如何构建？
- [构建说明（英文）](https://github.com/nillerusr/source-engine/wiki/Source-Engine-(EN))
- [构建说明（俄文）](https://github.com/nillerusr/source-engine/wiki/Source-Engine-(RU))

# 支持我
BTC: `bc1qnjq92jj9uqjtafcx2zvnwd48q89hgtd6w8a6na`

ETH: `0x5d0D561146Ed758D266E59B56e85Af0b03ABAF46`

XMR: `48iXvX61MU24m5VGc77rXQYKmoww3dZh6hn7mEwDaLVTfGhyBKq2teoPpeBq6xvqj4itsGh6EzNTzBty6ZDDevApCFNpsJ`
