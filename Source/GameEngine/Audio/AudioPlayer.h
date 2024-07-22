#pragma once
#include <string>

namespace FMOD
{
    namespace Studio
    {
        class EventInstance;
    }
}

class AudioPlayer
{
public:
    ~AudioPlayer();
    void Initialize(std::string aEventName);
    void Play();
    void Stop(bool aFadeOut = true);
    void Pause();
    void Unpause();
    void SetVolume(float aVolume);
    float GetVolume();
protected:
    FMOD::Studio::EventInstance* myEventInstance;
};

