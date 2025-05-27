#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "GameEngine/ComponentSystem/GameObjectEventType.h"
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

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

    void Play(const std::string& aAudioName);
    void AddAudioInstance(const std::string& aFMODEventName, bool aIsOneShot, SourceType aSourceType = SourceType::Non3D, bool aPlayOnStart = false);
    void SetAudioPlayOnEvent(const std::string& aAudioName, GameObjectEventType aEventType);

    bool Serialize(nl::json& outJsonObject) override;
    bool Deserialize(nl::json& aJsonObject) override;
private:
    void Update3DLocation(std::shared_ptr<AudioInstance> aInstance);
    std::unordered_map<std::string, AudioInstanceData> myAudioInstances;
    std::unordered_map<GameObjectEventType, std::vector<std::string>> myPlayOnEvents;
};

