#pragma once
#include <string>

namespace FMOD
{
    namespace Studio
    {
        class EventInstance;
    }
}

class AudioInstance
{
public:
    ~AudioInstance();
    bool Initialize(std::string aEventName);
    void Play();
    void Stop(bool aFadeOut = true);
    void Pause();
    void Unpause();
    void SetVolume(float aVolume);
    float GetVolume();
    std::string GetEventName() { return myEventName; }
protected:
    FMOD::Studio::EventInstance* myEventInstance;
    std::string myEventName;
};

