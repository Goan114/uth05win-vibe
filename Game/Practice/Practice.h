#pragma once
#include <windows.h>
namespace th5w {
class CStage;
namespace practice {
enum Tool { Invincible, InfiniteLives, InfiniteBombs, KeepPower, LockTimeout, AutoBomb, EnemyInvincible, ToolCount };
bool Enabled(Tool tool);
void Toggle(Tool tool);
void ResetRun();
bool Assisted();
void MarkAssisted();
bool AllowTimeout(bool expired);
bool BeforeTimeout(bool beforeDeadline);
bool Initialize(HWND window);
void Shutdown();
void BeginFrame();
void EndFrame();
void WindowMessage(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
void FocusPause();
void DrawPause(CStage& stage);
void DrawStatus();
}
}
