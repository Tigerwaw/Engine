#include "AudioInstance.h"
#include "fmod/fmod.hpp"
#include "fmod/fmod_studio.hpp"
#include "fmod/fmod_common.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Audio/AudioEngine.h"

#include "Logger/Logger.h"

#if _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogAudioPlayer, AudioInstance, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(LogAudioPlayer, AudioPlayer, Warning);
#endif

#define AUDIOLOG(Verbosity, Message, ...) LOG(LogAudioPlayer, Verbosity, Message, ##__VA_ARGS__)
DEFINE_LOG_CATEGORY(LogAudioPlayer);

AudioInstance::~AudioInstance()
{
    if (myEventInstance->isValid())
    {
        FMOD_RESULT result = myEventInstance->release();
        if (result != FMOD_OK)
        {
            AUDIOLOG(Error, "Failed to delete audio player");
        }
    }
}

bool AudioInstance::Initialize(std::string aEventName)
{
    FMOD::Studio::EventInstance* eventInstance = Engine::GetInstance().GetAudioEngine().CreateEventInstance(aEventName);
    if (!eventInstance)
    {
        AUDIOLOG(Error, "Could not initialize audio player {}", aEventName);
        return false;
    }

    myEventInstance = eventInstance;
    myEventName = aEventName;
    return true;
}

void AudioInstance::Play()
{
    myEventInstance->start();
}

void AudioInstance::Stop(bool aFadeOut)
{
    if (aFadeOut)
    {
        myEventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
    }
    else
    {
        myEventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
    }
}

void AudioInstance::Pause()
{
    myEventInstance->setPaused(true);
}

void AudioInstance::Unpause()
{
    myEventInstance->setPaused(false);
}

void AudioInstance::SetVolume(float aVolume)
{
    myEventInstance->setVolume(aVolume);
}

float AudioInstance::GetVolume()
{
    float volume = 0;
    myEventInstance->getVolume(&volume);
    return volume;
}
