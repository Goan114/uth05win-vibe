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
