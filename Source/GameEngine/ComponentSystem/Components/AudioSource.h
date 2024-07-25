#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "GameEngine/ComponentSystem/GameObjectEventType.h"
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>

class AudioInstance;

class AudioSource : public Component
{
public:
    enum SourceType
    {
        Non3D,
        AtLocation,
        Following
    };

    struct AudioInstanceData
    {
        std::shared_ptr<AudioInstance> instance;
        bool isOneShot = true;
        SourceType sourceType = SourceType::Non3D;
    };

    void Start() override;
    void Update() override;
    void ReceiveEvent(const GameObjectEvent& aEvent) override;

    void Play(std::string aAudioName);
    void AddAudioInstance(std::string aEventName, bool aIsOneShot, SourceType aSourceType = SourceType::Non3D);
    void AddAudioPlayOnEvent(std::string aAudioName, GameObjectEventType aEventType);
private:
    void Update3DLocation(std::shared_ptr<AudioInstance> aInstance);
    std::unordered_map<std::string, AudioInstanceData> myAudioInstances;
    std::unordered_map<GameObjectEventType, std::vector<std::string>> myPlayOnEvents;
};

