#include "AudioPlayer.h"
#include "fmod/fmod.hpp"
#include "fmod/fmod_studio.hpp"
#include "fmod/fmod_common.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Audio/AudioEngine.h"

#include "Logger/Logger.h"

#if _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogAudioPlayer, AudioPlayer, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(LogAudioPlayer, AudioPlayer, Warning);
#endif

#define AUDIOLOG(Verbosity, Message, ...) LOG(LogAudioPlayer, Verbosity, Message, ##__VA_ARGS__)
DEFINE_LOG_CATEGORY(LogAudioPlayer);

AudioPlayer::~AudioPlayer()
{
    FMOD_RESULT result = myEventInstance->release();
    if (result != FMOD_OK)
    {
        AUDIOLOG(Error, "Failed to delete audio player");
    }
}

void AudioPlayer::Initialize(std::string aEventName)
{
    myEventInstance = Engine::GetInstance().GetAudioEngine().CreateEventInstance(aEventName);
    if (!myEventInstance)
    {
        AUDIOLOG(Error, "Could not initialize audio player {}", aEventName);
    }
}

void AudioPlayer::Play()
{
    myEventInstance->start();
}

void AudioPlayer::Stop(bool aFadeOut)
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

void AudioPlayer::Pause()
{
    myEventInstance->setPaused(true);
}

void AudioPlayer::Unpause()
{
    myEventInstance->setPaused(false);
}

void AudioPlayer::SetVolume(float aVolume)
{
    myEventInstance->setVolume(aVolume);
}

float AudioPlayer::GetVolume()
{
    float volume = 0;
    myEventInstance->getVolume(&volume);
    return volume;
}
