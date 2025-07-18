#pragma once
#include "ComponentSystem/Component.h"
#include "Math/Vector.hpp"

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

    void Play(const std::string& aAudioName);
    void AddAudioInstance(const std::string& aFMODEventName, bool aIsOneShot, SourceType aSourceType = SourceType::Non3D, bool aPlayOnStart = false);

    bool Serialize(nl::json& outJsonObject) override;
    bool Deserialize(nl::json& aJsonObject) override;
private:
    void Update3DLocation(std::shared_ptr<AudioInstance> aInstance);
    std::unordered_map<std::string, AudioInstanceData> myAudioInstances;
};

