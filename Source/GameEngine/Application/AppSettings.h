#pragma once
class AppSettings
{
public:
    static void LoadSettings(const std::filesystem::path& aSettingsFilepath);
    static AppSettings& Get();
    static void Destroy();

    std::string title;
    std::filesystem::path contentRoot;
    CU::Vector2f resolution;
    CU::Vector2f windowSize;
    CU::Vector2f windowPos;
    bool isFullscreen = true;
    bool isBorderless = true;
    bool allowDropFiles = false;
    bool autoRegisterAssets = true;

private:
    AppSettings();
    ~AppSettings();
};