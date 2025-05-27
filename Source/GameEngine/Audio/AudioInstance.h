#pragma once

#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

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
    // Allowing overlap means that multiple internal audio instances will be automatically created to avoid cut-offs and overlaps of frequent sounds like footsteps or gunshots.
    // This should be off for looping sounds like music or ambience.
    bool Initialize(const std::string& aEventName, bool aAllowOverlap);
    void Play();
    void Stop(bool aFadeOut = true);
    void Pause();
    void Unpause();
    void SetVolume(float aVolume);
    void Set3dPosition(CU::Vector3f aPosition, CU::Vector3f aVelocity = { 0, 0, 0 }, CU::Vector3f aForward = { 0, 0, 1.0f }, CU::Vector3f aUp = { 0, 1.0f, 0 });
    const float GetVolume() const;
    const bool GetIsPlaying() const;
    const std::string& GetEventName() const { return myEventName; }
protected:
    void ClearAllInstances();
    void AddInstance();
    std::vector<FMOD::Studio::EventInstance*> myEventInstances;
    std::string myEventName;
    bool myAllowOverlap = true;
};

