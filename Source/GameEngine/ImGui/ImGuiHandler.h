#pragma once
#include <windows.h>

class ImGuiHandler
{
public:
    void Initialize(HWND aMainWindowHandle);
    void Destroy();
    void BeginFrame();
    void Render();
    void Update();
private:
};

