#include "Enginepch.h"

#include "AudioInstance.h"
#include "Engine.h"
#include "Audio/AudioEngine.h"

AudioInstance::~AudioInstance()
{
    ClearAllInstances();
}

void AudioInstance::ClearAllInstances()
{
    for (auto& eventInstance : myEventInstances)
    {
        if (!eventInstance->isValid()) continue;
        
        FMOD_RESULT result = eventInstance->release();
        if (result != FMOD_OK)
        {
            LOG(LogAudioPlayer, Error, "Failed to delete audio instance {}", myEventName);
        }
    }

    myEventInstances.clear();
}

void AudioInstance::AddInstance()
{
    FMOD::Studio::EventInstance* eventInstance = Engine::Get().GetAudioEngine().CreateEventInstance(myEventName);
    if (!eventInstance)
    {
        LOG(LogAudioPlayer, Error, "Could not initialize audio instance {}", myEventName);
        return;
    }

    myEventInstances.emplace_back(eventInstance);
    LOG(LogAudioPlayer, Log, "Added an audio instance of {}", myEventName);
}

bool AudioInstance::Initialize(const std::string& aEventName, bool aAllowOverlap)
{
    FMOD::Studio::EventInstance* eventInstance = Engine::Get().GetAudioEngine().CreateEventInstance(aEventName);
    if (!eventInstance)
    {
        LOG(LogAudioPlayer, Error, "Could not initialize audio instance {}", aEventName);
        return false;
    }

    ClearAllInstances();

    myEventInstances.emplace_back(eventInstance);
    myEventName = aEventName;
    myAllowOverlap = aAllowOverlap;
    return true;
}

void AudioInstance::Play()
{
    for (auto& instance : myEventInstances)
    {
        FMOD_STUDIO_PLAYBACK_STATE playbackState;
        FMOD_RESULT result = instance->getPlaybackState(&playbackState);
        if (result != FMOD_OK)
        {
            LOG(LogAudioPlayer, Error, "Failed to get playback state of audio instance {}", myEventName);
            continue;
        }

        if (playbackState == FMOD_STUDIO_PLAYBACK_STOPPED)
        {
            instance->start();
            return;
        }
    }

    if (myAllowOverlap)
    {
        // Could not find a free audio instance to play, adding an additional instance.
        AddInstance();
    }
}

void AudioInstance::Stop(bool aFadeOut)
{
    for (auto& instance : myEventInstances)
    {
        if (aFadeOut)
        {
            instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
        }
        else
        {
            instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
        }
    }
}

void AudioInstance::Pause()
{
    for (auto& instance : myEventInstances)
    {
        instance->setPaused(true);
    }
}

void AudioInstance::Unpause()
{
    for (auto& instance : myEventInstances)
    {
        instance->setPaused(false);
    }
}

void AudioInstance::SetVolume(float aVolume)
{
    for (auto& instance : myEventInstances)
    {
        instance->setVolume(aVolume);
    }
}

void AudioInstance::Set3dPosition(Math::Vector3f aPosition, Math::Vector3f aVelocity, Math::Vector3f aForward, Math::Vector3f aUp)
{
    const FMOD_3D_ATTRIBUTES attributes = { static_cast<FMOD_VECTOR>(aPosition), static_cast<FMOD_VECTOR>(aVelocity), static_cast<FMOD_VECTOR>(aForward), static_cast<FMOD_VECTOR>(aUp) };

    for (auto& instance : myEventInstances)
    {
        FMOD_RESULT result = instance->set3DAttributes(&attributes);
        if (result != FMOD_OK)
        {
            LOG(LogAudioPlayer, Error, "Failed to set 3d attributes of audio instance {}", myEventName);
            continue;
        }
    }
}

const bool AudioInstance::GetIsPlaying() const
{
    for (auto& instance : myEventInstances)
    {
        FMOD_STUDIO_PLAYBACK_STATE playbackState;
        FMOD_RESULT result = instance->getPlaybackState(&playbackState);
        if (result != FMOD_OK)
        {
            LOG(LogAudioPlayer, Error, "Failed to get playback state of audio instance {}", myEventName);
            continue;
        }

        switch (playbackState)
        {
        case FMOD_STUDIO_PLAYBACK_PLAYING:
            LOG(LogAudioPlayer, Log, "Playing");
            return true;
            break;
        case FMOD_STUDIO_PLAYBACK_SUSTAINING:
            LOG(LogAudioPlayer, Log, "Sustaining");
            break;
        case FMOD_STUDIO_PLAYBACK_STOPPED:
            LOG(LogAudioPlayer, Log, "Stopped");
            break;
        case FMOD_STUDIO_PLAYBACK_STARTING:
            LOG(LogAudioPlayer, Log, "Starting");
            break;
        case FMOD_STUDIO_PLAYBACK_STOPPING:
            LOG(LogAudioPlayer, Log, "Stopping");
            break;
        case FMOD_STUDIO_PLAYBACK_FORCEINT:
            LOG(LogAudioPlayer, Log, "Forceint");
            break;
        default:
            break;
        }
    }

    return false;
}

const float AudioInstance::GetVolume() const
{
    if (myEventInstances.empty()) return 0;
    if (!myEventInstances[0]) return 0;

    float volume = 0;
    myEventInstances[0]->getVolume(&volume);
    return volume;
}
