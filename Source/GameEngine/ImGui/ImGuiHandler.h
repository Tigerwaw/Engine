#pragma once

class ImGuiHandler
{
public:
    void Initialize(HWND aMainWindowHandle);
    void Destroy();
    void BeginFrame();
    void Render();
    void Update();
private:
    void Settings();
    void Lighting();
    void Performance();
    void Controls();
    void WindowSize();
};

