#include "Enginepch.h"
#include "AppSettings.h"

static AppSettings* sInstance = nullptr;

void AppSettings::LoadSettings(const std::filesystem::path& aSettingsFilepath)
{
    assert(!sInstance);
    sInstance = new AppSettings();
    AppSettings& instance = *sInstance;

    std::ifstream path(aSettingsFilepath);
    nl::json data = nl::json();

    try
    {
        data = nl::json::parse(path);
    }
    catch (nl::json::parse_error e)
    {
        LOG(LogGameEngine, Error, "Couldn't read application settings file, {}!", e.what());
        return;
    }
    path.close();

    if (data.contains("title"))
    {
        std::string title = data["title"].get<std::string>();
        instance.title = title;
    }

    if (data.contains("assetsDir"))
    {
        std::filesystem::path assetsDir = data["assetsDir"].get<std::string>();
        instance.contentRoot = assetsDir;
    }

    if (data.contains("resolution"))
    {
        instance.resolution = { data["resolution"]["width"].get<float>(), data["resolution"]["height"].get<float>() };
    }

    if (data.contains("windowSize"))
    {
        instance.windowSize = { data["windowSize"]["width"].get<float>(), data["windowSize"]["height"].get<float>() };
    }

    if (data.contains("windowPos"))
    {
        instance.windowPos = { data["windowPos"]["top"].get<float>(), data["windowPos"]["left"].get<float>() };
    }

    if (data.contains("fullscreen"))
    {
        instance.isFullscreen = data["fullscreen"].get<bool>();
    }

    if (data.contains("borderless"))
    {
        instance.isBorderless = data["borderless"].get<bool>();
    }

    if (data.contains("allowdropfiles"))
    {
        instance.allowDropFiles = data["allowdropfiles"].get<bool>();
    }

    if (data.contains("autoregisterassets"))
    {
        instance.autoRegisterAssets = data["autoregisterassets"].get<bool>();
    }

    LPWSTR* szArgList;
    int argCount;
    szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);

    for (int i = 0; i < argCount; i++)
    {
        std::wstring arg = std::wstring(szArgList[i]);

        if (arg.find(L"FULLSCREEN") != std::string::npos)
        {
            arg.erase(0, 11);
            instance.isFullscreen = static_cast<bool>(std::stoi(arg, nullptr));
        }
        if (arg.find(L"BORDERLESS") != std::string::npos)
        {
            arg.erase(0, 11);
            instance.isBorderless = static_cast<bool>(std::stoi(arg, nullptr));
        }
        if (arg.find(L"WINPOSX") != std::string::npos)
        {
            arg.erase(0, 8);
            instance.windowPos.x = std::stof(arg, nullptr);
        }
        if (arg.find(L"WINPOSY") != std::string::npos)
        {
            arg.erase(0, 8);
            instance.windowPos.y = std::stof(arg, nullptr);
        }
        if (arg.find(L"WINSIZEX") != std::string::npos)
        {
            arg.erase(0, 9);
            instance.windowSize.x = std::stof(arg, nullptr);
            instance.resolution.x = instance.windowSize.x;
        }
        if (arg.find(L"WINSIZEY") != std::string::npos)
        {
            arg.erase(0, 9);
            instance.windowSize.y = std::stof(arg, nullptr);
            instance.resolution.y = instance.windowSize.y;
        }
    }
}

AppSettings& AppSettings::Get()
{
    assert(sInstance);
    return *sInstance;
}

void AppSettings::Destroy()
{
    assert(sInstance);
    delete sInstance;
}

AppSettings::AppSettings() = default;
AppSettings::~AppSettings() = default;
