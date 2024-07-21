#pragma once
#include "fmod/fmod.hpp"
#include "fmod/fmod_studio.hpp"
#include "fmod/fmod_common.h"
#include <memory>
#include <unordered_map>
#include <string>

class AudioEngine
{
public:
    void Initialize();
    void Destroy();
    void Update();
    bool LoadBank(std::string aPath);
    bool LoadEvent(std::string aPath);
    bool PlayEvent(std::string aPath);
private:
    FMOD::Studio::System* mySystem = nullptr;
    std::unordered_map<std::string, FMOD::Studio::Bank*> myBanks;
    std::unordered_map<std::string, FMOD::Studio::EventInstance*> myEvents;
};