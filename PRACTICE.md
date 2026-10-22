# 原生 Practice Start

本工作树位于 `native-practice` 分支，保留原版游戏和新增原生 Practice Start 菜单。游戏使用原生暂停菜单，构建不依赖 ImGui，不包含 F1–F6、U 等作弊功能。

标题 Extra Start 下方进入 Practice Start，选角后弹出游戏字形的黑底选关窗口。上下选择，Z 开始，X／Esc 收拢窗口返回。关卡按角色、难度分别解锁，正常实战进入对应关卡后写入 game.cfg；已有整作通关记录用于迁移。

单关初始 8 个备用残机、8B 和满火力。通关或耗尽生命直接进入原生 Replay 保存页，沿用单关随机种子和输入流，播放结束不续接下一关。

构建：`.\scripts\Build-Practice.ps1 -GameDirectory "游戏资源目录"`，输出 `Release/uth05win-practice.exe`。

验证：`.\scripts\Test-Practice.ps1 -GameDirectory "游戏资源目录"`。在独立 sandbox 内检查菜单、解锁、窗口开关、单关分流及六关录像逐帧回放。

带 thprac 的分支为 `thprac`，工作目录 `C:\Users\w3051\Desktop\uth05win`。

分支验收：纯原生构建与 84 项 Practice 检查通过。

## 输入、解锁存档与保存录像

游戏窗口禁用输入法组合界面，主菜单练习提示为“开始单关练习”。正常实战进入对应关卡即解锁该角色／难度的练习，Practice 和录像不触发解锁。

解锁状态写入原生 game.cfg 的版本化扩展区，保留前 52 字节设置字段；支持读取原 52 字节存档，并迁移先前 practice-progress.dat 的数据。新增进度不再写单独的解锁文件。

普通 Practice 通关直接打开原生录像保存页，名字表 END 执行保存。带 thprac 版本也保留保存入口；临时 F1–F6／U 开关不写入录像，未来的大 Practice 初始参数将另行接入录像状态。

## 原生 Music Room

主菜单 Music Room 进入旧五作音乐室：左右切换作品，上下选曲并滚动，Z／Enter 重新播放所选曲，Esc 或列表底部返回标题。默认从 TH05 开始；五组依次有 14、18、24、28、23 首，共 107 首。播放曲目使用单独的高亮色，选择框可以在播放期间浏览其他曲目。

布局依据 ReC98 提交 `b6ba5b0a529edbb31efdf8c0e939263804f8ee47` 的 `th02/op/m_music.cpp`（GAME=5）、`th05/shiftjis/music.hpp` 和 `th05/op/piano_c.cpp`：12 行曲目列表、原 MUSIC.PI 背景、14 个运动多边形（仅设置 B 色平面，保留底图点阵）、原版逐曲空格排版、右侧曲名与九行评论、FM1–5 和 SSG 钢琴。钢琴读取 PMDWin 当前声部的音符与门限，不生成虚拟音符。

汉化音乐评论以原始定长字节记录读取。音乐室专用字库会把繁体字映射为简体，并用 16×16 宋体点阵重绘变更的字形，保留原 ASCII、假名、逐曲排版与字节宽度；不改写原资源，也不影响游戏主字库。资源导入工具只读原 HDI，向游戏目录写入 `music.dat` 与 `music-font.bmp`；资源不进入源码提交：

```powershell
python scripts/prepare-music-room.py --source "原版汉化版目录" --game "Windows游戏资源目录"
```

`--source` 目录包含 `thow_cn.hdi` 与 `f-c1.bmp`。缺少音乐室资源时入口禁用，其他游戏功能照常使用。两个分支均提供 Music Room，且不依赖 ImGui。

参考：https://github.com/nmlgc/ReC98/blob/master/th02/op/m_music.cpp 。测试覆盖全部 107 首实际 PMD 加载、定长评论、切换作品、12 行滚动、循环选择、播放高亮、返回标题和钢琴音符范围；截图位于测试 sandbox。
