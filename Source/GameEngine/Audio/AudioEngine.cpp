#include "AudioEngine.h"
#include "GameEngine/EngineSettings.h"

#include "Logger/Logger.h"

#if _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogAudioEngine, AudioEngine, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(LogAudioEngine, AudioEngine, Warning);
#endif

#define AUDIOLOG(Verbosity, Message, ...) LOG(LogAudioEngine, Verbosity, Message, ##__VA_ARGS__)
DEFINE_LOG_CATEGORY(LogAudioEngine);

void AudioEngine::Initialize()
{
    static FMOD_RESULT result = mySystem->create(&mySystem, FMOD_VERSION);
    if (result != FMOD_OK)
    {
        AUDIOLOG(Error, "Failed to create fmod system with code {}", static_cast<int>(result));
        return;
    }

    result = mySystem->initialize(FMOD_MAX_CHANNEL_WIDTH, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
    if (result != FMOD_OK)
    {
        AUDIOLOG(Error, "Failed to initialize fmod system with code {}", static_cast<int>(result));
        return;
    }

    myContentRoot = EngineSettings::GetContentRootPath().string() + "AudioBanks/";
    AUDIOLOG(Log, "Successfully initialized fmod system");
}

void AudioEngine::Destroy()
{
    FMOD_RESULT result = mySystem->release();
    if (result != FMOD_OK)
    {
        AUDIOLOG(Error, "Failed to release fmod system with code {}", static_cast<int>(result));
        return;
    }

    AUDIOLOG(Log, "Successfully released and deleted fmod system");
}

void AudioEngine::Update()
{
    if (!mySystem)
    {
        return;
    }

    mySystem->update();
}

bool AudioEngine::LoadBank(std::string aBankName)
{
    auto bankIterator = myBanks.find(aBankName);
    if (bankIterator != myBanks.end())
    {
        AUDIOLOG(Warning, "Audio bank {} is already loaded", aBankName);
        return true;
    }

    FMOD::Studio::Bank* newBank = nullptr;
    FMOD_RESULT result = mySystem->loadBankFile((myContentRoot + aBankName + ".bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &newBank);
    if (result != FMOD_OK)
    {
        AUDIOLOG(Error, "Failed to load audio bank {}", aBankName);
        return false;
    }

    myBanks.emplace(aBankName, newBank);
    AUDIOLOG(Log, "Successfully loaded audio bank {}", aBankName);
    LoadBankEvents(aBankName);
    return true;
}

bool AudioEngine::UnloadBank(std::string aBankFileName)
{
    FMOD_RESULT result = myBanks[aBankFileName]->unload();
    if (result != FMOD_OK)
    {
        AUDIOLOG(Error, "Failed to unload audio bank {}", aBankFileName);
        return false;
    }

    return true;
}

FMOD::Studio::EventInstance* AudioEngine::CreateEventInstance(std::string aEventName)
{
    auto eventIterator = myEvents.find(aEventName);
    if (eventIterator == myEvents.end())
    {
        AUDIOLOG(Warning, "Can't find event description {}", aEventName);
        return nullptr;
    }

    FMOD::Studio::EventInstance* eventInstance = nullptr;
    FMOD_RESULT result = myEvents[aEventName]->createInstance(&eventInstance);
    if (result != FMOD_OK)
    {
        AUDIOLOG(Error, "Failed to create event instance of {}", aEventName);
        return nullptr;
    }

    AUDIOLOG(Log, "Create event instance of {}", aEventName);
    return eventInstance;
}

bool AudioEngine::LoadBankEvents(std::string aBankName)
{
    int eventCount = 0;
    myBanks[aBankName]->getEventCount(&eventCount);
    std::vector<FMOD::Studio::EventDescription*> eventDescs;
    eventDescs.resize(eventCount);
    myBanks[aBankName]->getEventList(eventDescs.data(), eventCount, nullptr);

    if (eventCount == 0)
    {
        return false;
    }

    for (auto& eventDesc : eventDescs)
    {
        std::string eventPath;
        char* charPath = eventPath.data();
        FMOD_RESULT result = eventDesc->getPath(charPath, 256, nullptr);
        if (result != FMOD_OK)
        {
            AUDIOLOG(Error, "Failed to load event path with code {}", static_cast<int>(result));
            return false;
        }

        eventPath = charPath;
        if (eventPath == "")
        {
            AUDIOLOG(Error, "Event path is empty and has been discarded");
            return false;
        }

        std::string cleanedPath = eventPath.substr(strlen("event:/"));
        AUDIOLOG(Log, "Loaded Event Description {}", cleanedPath);
        myEvents.emplace(cleanedPath, eventDesc);
    }

    AUDIOLOG(Log, "Successfully loaded all {} events in audio bank {}", eventCount, aBankName);
    return true;
}
