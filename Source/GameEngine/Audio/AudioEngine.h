#pragma once
#include <memory>
#include <unordered_map>
#include <string>

namespace FMOD
{
    namespace Studio
    {
        class System;
        class Bank;
        class EventDescription;
        class EventInstance;
    }
}

class AudioEngine
{
public:
    void Initialize();
    void Destroy();
    void Update();
    bool LoadBank(std::string aBankName);
    bool UnloadBank(std::string aBankName);

    FMOD::Studio::EventInstance* CreateEventInstance(std::string aEventName);
private:
    bool LoadBankEvents(std::string aBankName);
    std::string myContentRoot;
    FMOD::Studio::System* mySystem = nullptr;
    std::unordered_map<std::string, FMOD::Studio::Bank*> myBanks;
    std::unordered_map<std::string, FMOD::Studio::EventDescription*> myEvents;
};