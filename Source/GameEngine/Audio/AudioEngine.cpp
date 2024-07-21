#include "AudioEngine.h"

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

bool AudioEngine::LoadBank(std::string aPath)
{
    FMOD::Studio::Bank* newBank = nullptr;
    FMOD_RESULT result = mySystem->loadBankFile(aPath.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &newBank);
    if (result != FMOD_OK)
    {
        AUDIOLOG(Error, "Failed to load audio bank at {}", aPath);
        return false;
    }

    myBanks.emplace(aPath, newBank);

    AUDIOLOG(Log, "Successfully loaded audio bank {}", aPath);
    return true;
}

bool AudioEngine::LoadEvent(std::string aPath)
{
    FMOD::Studio::EventDescription* eventDesc = nullptr;
    FMOD_RESULT result = mySystem->getEvent(aPath.c_str(), &eventDesc);
    if (result != FMOD_OK)
    {
        AUDIOLOG(Error, "Failed to load event description {}", aPath);
        return false;
    }

    FMOD::Studio::EventInstance* eventInstance = nullptr;
    result = eventDesc->createInstance(&eventInstance);
    if (result != FMOD_OK)
    {
        AUDIOLOG(Error, "Failed to load event instance {}", aPath);
        return false;
    }

    myEvents.emplace(aPath, eventInstance);

    AUDIOLOG(Log, "Successfully loaded audio event {}", aPath);
    return true;
}

bool AudioEngine::PlayEvent(std::string aPath)
{
    FMOD_RESULT result = myEvents[aPath]->start();
    if (result != FMOD_OK)
    {
        AUDIOLOG(Error, "Failed to play event {}", aPath);
        return false;
    }

    return true;
}
