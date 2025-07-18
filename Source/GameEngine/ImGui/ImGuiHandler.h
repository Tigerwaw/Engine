#pragma once

class ImGuiHandler
{
public:
    void Initialize(HWND aMainWindowHandle);
    void Destroy();
    void BeginFrame();
    void Render();
};

