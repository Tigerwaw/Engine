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
        class Bus;
        class EventDescription;
        class EventInstance;
    }
}

class GameObject;

enum BusType
{
    SFX,
    Music,
    Ambience
};

class AudioEngine
{
public:
    void Initialize();
    void Destroy();
    void Update();
    void SetListener(std::shared_ptr<GameObject> aGameObject);
    bool LoadBank(std::string aBankName);
    bool UnloadBank(std::string aBankName);
    bool AddBus(BusType aBusType, std::string aBusName);
    void SetVolumeOfBus(BusType aBusType, float aVolume);
    const float GetVolumeOfBus(BusType aBusType) const;
    void IncreaseVolumeOfBus(BusType aBusType, float aVolumeIncrease);
    void DecreaseVolumeOfBus(BusType aBusType, float aVolumeDecrease);

    FMOD::Studio::EventInstance* CreateEventInstance(std::string aEventName);
private:
    bool LoadBankEvents(std::string aBankName);
    void UpdateListener();
    std::string myContentRoot;
    FMOD::Studio::System* mySystem = nullptr;
    std::unordered_map<std::string, FMOD::Studio::Bank*> myBanks;
    std::unordered_map<BusType, FMOD::Studio::Bus*> myBuses;
    std::unordered_map<std::string, FMOD::Studio::EventDescription*> myEvents;
    std::shared_ptr<GameObject> myListener;

    bool myHasWarnedAboutListenerError;
};