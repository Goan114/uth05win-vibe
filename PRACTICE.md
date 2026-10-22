# uth05win Practice 工具

Windows 原生 ImGui 接入，界面依据本地紫版 thprac 的现有实现移植。

- Backspace：开关左上角 `Mod Menu`。每项为可点击文字；启用时为绿色 `[按键: 功能]`，F1–F6 和 U 也可直接切换。
- Esc：打开暂停菜单，方向键选择，Z／Enter 确认，鼠标可点击。Esc 继续、Q 不存录像退出、R 重新开始。
- 暂停菜单为继续游戏、退出游戏、不存 rep 退出游戏、重新开始。普通“退出游戏”进入原生录像保存流程；录像允许保存，临时作弊开关不写入录像。
- 不添加 F7，也不提前添加大 Practice 面板及其“调整选项”入口。

暂停窗口沿用 `TH06::THPauseMenu` 的 `(32,16)`、`384×448`、透明度 `0.8`、无标题栏／边框、140 像素上间隔、119 像素缩进和 `130×25` 按钮。作弊列表沿用 `THOverlay` 的 `(10,10)`、自动尺寸、透明度 `0.5`，以及 `GuiHotKey::OnWidgetUpdate` 的文本、绿色启用状态和 InvisibleButton 点击区域。暂停开关沿用原实现的淡入淡出及确认输入处理。

| 按键 | 功能 | 行为 |
| --- | --- | --- |
| F1 | 无敌 | 撞弹保留掉残音效和短暂无敌，不触发死亡或损失资源。 |
| F2 | 无限残机 | 正常 Miss 和复活，残机不扣减，最后一命也不进入 Game Over。 |
| F3 | 无限 Bomb | 正常使用 Bomb，不扣库存，库存为0时也可使用。 |
| F4 | 火力不减 | Miss 后保留当前火力。 |
| F5 | 锁定 Boss 超时 | Boss／中 Boss 攻击继续推进，禁止因时间或循环耗尽结束阶段，击破仍正常换段。 |
| F6 | 自动 Bomb | 接收到中弹判定后，在死亡流程开始前尝试正常 Bomb；库存与禁止 Bomb 条件仍生效。 |
| U | 敌人无敌 | 自机攻击不对敌人造成伤害，敌人的碰撞与弹幕继续运行。 |
| Esc | 暂停菜单 | 按紫版现有菜单绘制继续、退出、不存录像退出、重新开始。 |

本轮不包含 thprac 大 Practice 配置面板；原生 Practice Start 选关见下。录像播放不施加实时作弊；使用作弊的本局保留录像保存入口，但不写普通通关／排行榜记录。工具开关在同一程序会话中保留，重新启动默认关闭。

## 原生 Practice Start

标题中位于 Extra Start 之后、Replay 之前。使用当前 Options 难度进入原生四角色选择，确认角色后，在选角画面上显示黑底像素边框的选关窗口。

选关沿用 TH06 `MainMenu::ChoosePracticeLevel` 的六行 STAGE 列表、24px 行距和选中／未选中文字色；绘制用 TH05 的 `CCommonFunctionGraphic` 和 `CTh5ExtFont`（成绩录入使用的 ZUN.COM 外字），包含数字，不使用 ImGui／系统字体。上下循环选择 STAGE 1–6，Z 确认，X／Esc 回到选角。关卡按角色及难度分别解锁，正常实战进入对应关卡后即可选择；未进入过的关卡显示灰色 LOCKED。录像、Practice 和 thprac 作弊局不会解锁。已有完整通关记录会迁移为该角色／难度的六关已完成。进度保存在 game.cfg 的版本化扩展区，原有 52 字节设置字段保持原布局。分数栏当前为零，不包含独立的练习最高分存储。

每次开始或重试该单关，初始满火力、8 个备用残机和 8B。原生引擎把当前自机也计入生命，因此内部生命值为 9，HUD 显示 8。不能续关，正常 Miss 后按练习初始设置补充 Bomb。结束该关（含第六关）直接进入原生录像保存流程，不进入下一关、结局或普通排行榜；耗尽生命同样转保存页面。

沿用原生 RPY 格式的 `m_bPracticeMode`、单关输入流、初始资源和随机种子。保存后在 Replay 菜单对应关卡列播放；该关结束即退出播放。普通 Practice 本身不算 thprac 作弊，允许保存；F1–F6／U 属于临时工具，不作为录像初始参数序列化。

验证：136 项生产代码检查通过，包括六关分别运行 1200 帧的移动、低速及射击输入，保存 RPY、重新载入并逐帧比较帧号、坐标、火力、资源和分数；另检查通关／Game Over 保存分流、回放单关结束、选关返回及原生窗口截图。该检查不等同于完整打通六关。

## 构建

需要 Visual Studio C++ Desktop、Windows 10 SDK 和 Python 3。ImGui v1.82 与 Win32／OpenGL2 backend 已固定在 `third_party/imgui`，许可证见该目录 `LICENSE.txt`。

```powershell
.\scripts\Build-Practice.ps1 -GameDirectory "你的 uth05win 游戏目录"
```

脚本使用游戏目录已有的 `glew32.dll` 和 `PMDWin.dll` 生成本地 x86 导入库，输出 `Release/uth05win-thprac.exe`。将该程序放进游戏资源目录运行；原 `uth05win.exe` 可保留。

## 验证

```powershell
.\scripts\Test-Practice.ps1 -GameDirectory "你的 uth05win 游戏目录"
```

使用原生产游戏对象、Win32、OpenGL2 和 ImGui，在 `artifacts/practice/sandbox` 的独立资源副本上检查热键、防重复切换、资源变化、自动 Bomb、超时后击破、录像隔离、菜单点击和渲染。截图写入该目录 `pause.bmp`，不修改原游戏目录的成绩和录像。

界面参考：`C:\Users\w3051\Desktop\eagler\thprac\thprac_purple`，提交 `17780056107a014e88d35fceb3ad86c121d94287`。

- `thprac/src/thprac/thprac_th06.cpp`：`THOverlay`、`THPauseMenu`。
- `thprac/src/thprac/thprac_gui_components.cpp`：`GameGuiWnd::Update`、`GuiHotKey::OnWidgetUpdate`、`GuiButton::operator()`、`GuiNavFocus`。
- `thprac/src/thprac/thprac_games.cpp`：默认 `StyleColorsDark` 和 640 像素宽度下 16px 字体。
- `thprac/src/thprac/thprac_games_def.json`：中文功能及按钮文案。

保留紫版的界面结构、控件和样式，替换内存 Hook 为本项目的源码状态操作，并使用 Win32／OpenGL2 后端。

ImGui 来源：<https://github.com/ocornut/imgui/tree/v1.82>。

选关窗口用 10 帧从中心向外展开并淡入，文字按展开范围裁切。X／Esc 返回时反向收拢并淡出，关闭完成后回到选角。Practice Start 使用随项目提供的 practice.bmp 与主菜单原生 Start 贴图组合。

## 分支与工作树

- `thprac`：本目录 `C:\Users\w3051\Desktop\uth05win`，包含原生 Practice Start、F1–F6／U 和紫版 ImGui 暂停界面；构建输出 `Release/uth05win-thprac.exe`。
- `native-practice`：`C:\Users\w3051\Desktop\uth05win-practice`，保留原生 Practice Start、解锁与录像，使用游戏原生暂停菜单，不包含作弊代码和 ImGui；输出 `Release/uth05win-practice.exe`。

两个工作树独立编译，不共享构建输出和测试 sandbox。

## 输入、解锁存档与保存录像

游戏窗口禁用输入法组合界面，主菜单练习提示为“开始单关练习”。正常实战进入对应关卡即解锁该角色／难度的练习，Practice 和录像不触发解锁。

解锁状态写入原生 game.cfg 的版本化扩展区，保留前 52 字节设置字段；支持读取原 52 字节存档，并迁移先前 practice-progress.dat 的数据。新增进度不再写单独的解锁文件。

普通 Practice 通关直接打开原生录像保存页，名字表 END 执行保存。带 thprac 版本也保留保存入口；临时 F1–F6／U 开关不写入录像，未来的大 Practice 初始参数将另行接入录像状态。

## 原生 Music Room

主菜单 Music Room 进入旧五作音乐室：左右切换作品，上下选曲并滚动，Z／Enter 重新播放所选曲，Esc 或列表底部返回标题。默认从 TH05 开始；五组依次有 14、18、24、28、23 首，共 107 首。播放曲目使用单独的高亮色，选择框可以在播放期间浏览其他曲目。

布局依据 ReC98 提交 `b6ba5b0a529edbb31efdf8c0e939263804f8ee47` 的 `th02/op/m_music.cpp`（GAME=5）、`th05/shiftjis/music.hpp` 和 `th05/op/piano_c.cpp`：12 行曲目列表、原 MUSIC.PI 背景、14 个运动多边形（仅设置 B 色平面，保留底图点阵）、原版逐曲空格排版、右侧曲名与九行评论、FM1–5 和 SSG 钢琴。钢琴读取 PMDWin 当前声部的音符与门限，不生成虚拟音符。

汉化音乐评论以原始定长字节记录读取，由指定汉化版的字库绘制。资源导入工具只读原 HDI，向游戏目录写入 `music.dat` 与 `music-font.bmp`；资源不进入源码提交：

```powershell
python scripts/prepare-music-room.py --source "原版汉化版目录" --game "Windows游戏资源目录"
```

`--source` 目录包含 `thow_cn.hdi` 与 `f-c1.bmp`。缺少音乐室资源时入口禁用，其他游戏功能照常使用。两个分支均提供 Music Room，且不依赖 ImGui。

参考：https://github.com/nmlgc/ReC98/blob/master/th02/op/m_music.cpp 。测试覆盖全部 107 首实际 PMD 加载、定长评论、切换作品、12 行滚动、循环选择、播放高亮、返回标题和钢琴音符范围；截图位于测试 sandbox。
