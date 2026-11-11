# 现代主菜单 / 暂停菜单 · 实施计划

## 1. 目标

在 `gameui` 模块内实现以下交互（不依赖游戏侧内容文件即可编译运行）：

1. **主菜单**（`!GameUI().IsInLevel()`）
   - 打开控制台时：两张 CARD（NEWS / PLAYER）先向右滑出并淡出，随后控制台淡入。
   - 关闭控制台时：控制台先淡出，两张 CARD 再滑入并淡入。
2. **暂停菜单**（`GameUI().IsInLevel()`）
   - 打开控制台时：菜单整体移动到屏幕左侧，控制台淡入（两者并行）。
   - 关闭控制台时：反向移动回原位（两者并行）。
3. 暂停菜单不显示比分（比分仅出现在设计稿 mockup 中，代码里没有该行，无需删除；后续若新增上下文行也不加比分）。

## 2. 现状

- 主菜单 = `gameui/BasePanel.cpp` 中的 `CBasePanel` + `CGameMenu`（`vgui::Menu`）。
- 暂停菜单 = 同一个 `CBasePanel` 在 `IsInLevel()` 为真时显示（没有独立的 pause panel）。
- 控制台 = `gameui/GameConsoleDialog.cpp` 的 `CGameConsoleDialog`，是独立的顶层面板，位于屏幕右半部分。
- 当前没有任何 CARD、也没有控制台淡入/菜单位移逻辑。

## 3. 方案

### 3.1 新增文件

| 文件 | 内容 |
|------|------|
| `gameui/ModernMenu.h` | `CModernMenuTween`（时间缓动）、`CModernMenuCard`（卡片面板） |
| `gameui/ModernMenu.cpp` | 上述实现 |

### 3.2 修改文件

| 文件 | 改动 |
|------|------|
| `gameui/BasePanel.h` | 新增 CARD 成员、控制台状态、菜单位移动画成员与方法声明 |
| `gameui/BasePanel.cpp` | 创建 CARD、布局、`RunFrame` 中驱动过渡、暂停菜单左移 |
| `gameui/GameConsoleDialog.h` | 覆写 `Activate/Hide/OnThink`，加入淡入淡出状态 |
| `gameui/GameConsoleDialog.cpp` | 控制台淡入（带延迟）与淡出（淡出完成后才真正隐藏） |
| `public/vgui_controls/consoledialog.h` | `Hide()` 改为 `virtual`（`Activate` 已是 virtual） |
| `gameui/wscript` | 加入 `ModernMenu.cpp` |
| `gameui/GameUI.vpc` | 加入 `ModernMenu.cpp/.h` |

### 3.3 状态机

**控制台面板（CGameConsoleDialog）**

```
隐藏 --Activate()--> FADE_IN (alpha 0，延迟 delay) --t--> 全显
全显 --Hide()------> FADE_OUT --t--> 真正隐藏
```

- `delay`：主菜单 0.16s（等 CARD 退场结束），暂停 0.06s
- 淡入 0.26s，淡出 0.20s，ease-out

**主菜单 CARD（CModernMenuCard）**

- 每张卡维护 0..1 的进度：0 = 隐藏（右移 24px + 透明），1 = 显示。
- 打开控制台：`AnimateTo(false)`，NEWS 0ms、PLAYER 40ms。
- 关闭控制台：`AnimateTo(true)`，NEWS 20ms、PLAYER 60ms。
- 进入关卡：`SnapTo(false)`（立即隐藏）。
- 回到主菜单（`OnGameUIActivated`）：`AnimateTo(true)`。

**暂停菜单位移**

- `m_flMenuShift` 0..1；0 = 原位，1 = 左侧。
- 目标 = `IsInLevel() && consoleOpen ? 1 : 0`，280ms ease-out。
- 打开时与控制台淡入并行；关闭时 `IsConsoleClosing()` 立即让 `consoleOpen` 变 false，因此菜单回位与控制台淡出并行。
- 在 `PerformLayout()` 中把位移同时应用到菜单、标题按钮、Logo。

### 3.4 触发方式

- `CBasePanel::RunFrame()` 每帧轮询 `GameConsole().IsConsoleVisible()`，检测到变化后启动对应动画。
- 这样同时覆盖菜单项打开控制台和 `~` 键打开控制台两条路径。

## 4. 配置（scheme，可选）

均带默认值，缺少时不影响运行：

| Key | 默认 |
|-----|------|
| `ModernMenu.CardBg` | `22 22 28 217` |
| `ModernMenu.CardTitle` | `242 242 245 255` |
| `ModernMenu.CardText` | `160 160 172 255` |
| `ModernMenu.Accent` | `77 216 232 255` |
| `ModernMenu.CardTitleFont` | `Default` |
| `ModernMenu.CardBodyFont` | `Default` |

卡片位置按屏幕尺寸自适应（右侧，距边 48，宽 360，高 132）。

## 5. 风险 / 回退

- 控制台淡出期间 `IsConsoleVisible()` 仍为 true，主菜单 CARD 会等淡出结束后再回归（比设计稿略晚，可接受）。
- 若某游戏内容里已有 `MainMenuOverride` 自定义菜单，本改动只影响引擎默认菜单；override 面板不受影响。
- 回退：删除 `ModernMenu.*`、还原 `BasePanel.*` 与 `GameConsoleDialog.*` 即可。

## 6. 验证

- 编译 `GameUI`。
- 主菜单：打开/关闭控制台，观察 CARD 与淡入顺序。
- 进入关卡后按 ESC 打开暂停菜单，打开/关闭控制台，观察菜单左移/归位。
- 确认暂停菜单无比分行。
