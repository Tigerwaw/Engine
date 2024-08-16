#pragma once

class ImGuiHandler
{
public:
    void Initialize(HWND aMainWindowHandle);
    void Destroy();
    void BeginFrame();
    void Render();
    void Update();
    void AddNewFunction(std::function<void()> aFunction);
private:

    std::vector<std::function<void()>> myLambdaList;
};

