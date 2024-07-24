#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "GameEngine/ComponentSystem/GameObjectEventType.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>

class AudioInstance;

class AudioSource : public Component
{
public:
    void Start() override;
    void Update() override;
    void ReceiveEvent(const GameObjectEvent& aEvent) override;

    void AddAudioInstance(std::string aEventName);
    void AddAudioPlayOnEvent(std::string aAudioName, GameObjectEventType aEventType);
private:
    std::unordered_map<std::string, std::shared_ptr<AudioInstance>> myAudioInstances;
    std::unordered_map<GameObjectEventType, std::vector<std::string>> myPlayOnEvents;
};

