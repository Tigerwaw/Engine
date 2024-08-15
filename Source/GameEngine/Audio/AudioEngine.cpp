#include "Enginepch.h"

#include "AudioEngine.h"
#include "GameEngine/Engine.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"

void AudioEngine::Initialize()
{
    static FMOD_RESULT result = mySystem->create(&mySystem, FMOD_VERSION);
    if (result != FMOD_OK)
    {
        LOG(LogAudioEngine, Error, "Failed to create fmod system with code {}", static_cast<int>(result));
        return;
    }

    result = mySystem->initialize(FMOD_MAX_CHANNEL_WIDTH, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
    if (result != FMOD_OK)
    {
        LOG(LogAudioEngine, Error, "Failed to initialize fmod system with code {}", static_cast<int>(result));
        return;
    }

    myContentRoot = Engine::GetInstance().GetContentRootPath().string() + "AudioBanks/";
    LOG(LogAudioEngine, Log, "Successfully initialized fmod system");
}

void AudioEngine::Destroy()
{
    FMOD_RESULT result = mySystem->release();
    if (result != FMOD_OK)
    {
        LOG(LogAudioEngine, Error, "Failed to release fmod system with code {}", static_cast<int>(result));
        return;
    }

    LOG(LogAudioEngine, Log, "Successfully released and deleted fmod system");
}

void AudioEngine::Update()
{
    if (!mySystem)
    {
        return;
    }

    UpdateListener();

    mySystem->update();
}

void AudioEngine::SetListener(GameObject* aGameObject)
{
    myListener = aGameObject;

}

void AudioEngine::UpdateListener()
{
    if (!myListener)
    {
        if (!myHasWarnedAboutListenerError)
        {
            LOG(LogAudioEngine, Warning, "No listener has been set!");
            myHasWarnedAboutListenerError = true;
        }
        
        return;
    }

    std::shared_ptr<Transform> transform = myListener->GetComponent<Transform>();
    if (!transform)
    {
        if (!myHasWarnedAboutListenerError)
        {
            LOG(LogAudioEngine, Error, "Listener does not contain a transform component!");
            myHasWarnedAboutListenerError = true;
        }

        return;
    }

    const FMOD_3D_ATTRIBUTES attributes = { static_cast<FMOD_VECTOR>(transform->GetTranslation()), {0, 0, 0}, static_cast<FMOD_VECTOR>(transform->GetForwardVector()), static_cast<FMOD_VECTOR>(transform->GetUpVector())};

    FMOD_RESULT result = mySystem->setListenerAttributes(0, &attributes);
    if (result != FMOD_OK)
    {
        if (!myHasWarnedAboutListenerError)
        {
            LOG(LogAudioEngine, Error, "Failed to set listener attributes!");
            myHasWarnedAboutListenerError = true;
        }

        return;
    }
}

bool AudioEngine::LoadBank(std::string aBankName)
{
    auto bankIterator = myBanks.find(aBankName);
    if (bankIterator != myBanks.end())
    {
        LOG(LogAudioEngine, Warning, "Audio bank {} is already loaded", aBankName);
        return true;
    }

    FMOD::Studio::Bank* newBank = nullptr;
    FMOD_RESULT result = mySystem->loadBankFile((myContentRoot + aBankName + ".bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &newBank);
    if (result != FMOD_OK)
    {
        LOG(LogAudioEngine, Error, "Failed to load audio bank {}", aBankName);
        return false;
    }

    myBanks.emplace(aBankName, newBank);
    LOG(LogAudioEngine, Log, "Successfully loaded audio bank {}", aBankName);
    LoadBankEvents(aBankName);
    return true;
}

bool AudioEngine::UnloadBank(std::string aBankFileName)
{
    FMOD_RESULT result = myBanks[aBankFileName]->unload();
    if (result != FMOD_OK)
    {
        LOG(LogAudioEngine, Error, "Failed to unload audio bank {}", aBankFileName);
        return false;
    }

    return true;
}

bool AudioEngine::AddBus(BusType aBusType, std::string aBusName)
{
    std::string path = "bus:/" + aBusName;
    FMOD::Studio::Bus* newBus = nullptr;
    FMOD_RESULT result = mySystem->getBus(path.c_str(), &newBus);
    if (result != FMOD_OK)
    {
        LOG(LogAudioEngine, Error, "Failed to add audio bus {}", aBusName);
        return false;
    }

    myBuses.emplace(aBusType, newBus);
    LOG(LogAudioEngine, Log, "Successfully added audio bus {}", aBusName);
    return true;
}

void AudioEngine::SetVolumeOfBus(BusType aBusType, float aVolume)
{
    auto busIterator = myBuses.find(aBusType);
    if (busIterator == myBuses.end())
    {
        LOG(LogAudioEngine, Warning, "Can't find audio bus type ", static_cast<unsigned>(aBusType));
        return;
    }

    myBuses.at(aBusType)->setVolume(aVolume);
}

const float AudioEngine::GetVolumeOfBus(BusType aBusType) const
{
    auto busIterator = myBuses.find(aBusType);
    if (busIterator == myBuses.end())
    {
        LOG(LogAudioEngine, Warning, "Can't find audio bus type ", static_cast<unsigned>(aBusType));
        return 0;
    }

    float aVolume = 0;
    myBuses.at(aBusType)->getVolume(&aVolume);
    return aVolume;
}

void AudioEngine::IncreaseVolumeOfBus(BusType aBusType, float aVolumeIncrease)
{
    auto busIterator = myBuses.find(aBusType);
    if (busIterator == myBuses.end())
    {
        LOG(LogAudioEngine, Warning, "Can't find audio bus type ", static_cast<unsigned>(aBusType));
        return;
    }

    float volume = 0;
    myBuses.at(aBusType)->getVolume(&volume);
    myBuses.at(aBusType)->setVolume(volume + aVolumeIncrease);
}

void AudioEngine::DecreaseVolumeOfBus(BusType aBusType, float aVolumeDecrease)
{
    auto busIterator = myBuses.find(aBusType);
    if (busIterator == myBuses.end())
    {
        LOG(LogAudioEngine, Warning, "Can't find audio bus type ", static_cast<unsigned>(aBusType));
        return;
    }

    float volume = 0;
    myBuses.at(aBusType)->getVolume(&volume);
    myBuses.at(aBusType)->setVolume(volume - aVolumeDecrease);
}

FMOD::Studio::EventInstance* AudioEngine::CreateEventInstance(std::string aEventName)
{
    auto eventIterator = myEvents.find(aEventName);
    if (eventIterator == myEvents.end())
    {
        LOG(LogAudioEngine, Warning, "Can't find event description {}", aEventName);
        return nullptr;
    }

    FMOD::Studio::EventInstance* eventInstance = nullptr;
    FMOD_RESULT result = myEvents[aEventName]->createInstance(&eventInstance);
    if (result != FMOD_OK)
    {
        LOG(LogAudioEngine, Error, "Failed to create event instance of {}", aEventName);
        return nullptr;
    }

    LOG(LogAudioEngine, Log, "Created event instance of {}", aEventName);
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
        eventPath.reserve(256);
        char* charPath = eventPath.data();
        FMOD_RESULT result = eventDesc->getPath(charPath, 256, nullptr);
        if (result != FMOD_OK)
        {
            LOG(LogAudioEngine, Error, "Failed to load event path with code {}", static_cast<int>(result));
            return false;
        }

        eventPath.assign(charPath);
        if (eventPath == "")
        {
            LOG(LogAudioEngine, Error, "Event path is empty and has been discarded");
            return false;
        }

        std::string cleanedPath = eventPath.substr(strlen("event:/"));
        LOG(LogAudioEngine, Log, "Loaded Event Description {}", cleanedPath);
        myEvents.emplace(cleanedPath, eventDesc);
    }

    LOG(LogAudioEngine, Log, "Successfully loaded all {} events in audio bank {}", eventCount, aBankName);
    return true;
}