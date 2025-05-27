#include "Enginepch.h"

#include "AudioSource.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/Audio/AudioInstance.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"

void AudioSource::Start()
{
}

void AudioSource::Update()
{
    for (auto& audioInstance : myAudioInstances)
    {
        if (audioInstance.second.sourceType == SourceType::Following)
        {
            Update3DLocation(audioInstance.second.instance);
        }
    }
}

void AudioSource::ReceiveEvent(const GameObjectEvent& aEvent)
{
    if (myPlayOnEvents.find(aEvent.type) == myPlayOnEvents.end()) return;

    for (auto& audioString : myPlayOnEvents.at(aEvent.type))
    {
        Play(audioString);
    }
}

void AudioSource::Play(const std::string& aAudioName)
{
    if (myAudioInstances.find(aAudioName) == myAudioInstances.end()) return;
    
    switch (myAudioInstances[aAudioName].sourceType)
    {
    case SourceType::Non3D:
        myAudioInstances[aAudioName].instance->Play();
        break;
    case SourceType::AtLocation:
        Update3DLocation(myAudioInstances[aAudioName].instance);
        myAudioInstances[aAudioName].instance->Play();
        break;
    case SourceType::Following:
        Update3DLocation(myAudioInstances[aAudioName].instance);
        myAudioInstances[aAudioName].instance->Play();
        break;
    }
}

void AudioSource::AddAudioInstance(const std::string& aFMODEventName, bool aIsOneShot, SourceType aSourceType, bool aPlayOnStart)
{
    std::shared_ptr<AudioInstance> newAudioInstance = std::make_shared<AudioInstance>();
    if (!newAudioInstance->Initialize(aFMODEventName, aIsOneShot)) return;

    AudioInstanceData newAudioData;
    newAudioData.instance = newAudioInstance;
    newAudioData.isOneShot = aIsOneShot;
    newAudioData.sourceType = aSourceType;
    myAudioInstances.emplace(aFMODEventName, newAudioData);

    if (aPlayOnStart)
    {
        Play(aFMODEventName);
    }
}

void AudioSource::SetAudioPlayOnEvent(const std::string& aAudioName, GameObjectEventType aEventType)
{
    if (myAudioInstances.find(aAudioName) == myAudioInstances.end()) return;

    if (myPlayOnEvents.find(aEventType) == myPlayOnEvents.end())
    {
        myPlayOnEvents.emplace(aEventType, std::vector<std::string>());
    }

    for (auto& audioString : myPlayOnEvents.at(aEventType))
    {
        if (audioString == aAudioName) return;
    }

    myPlayOnEvents.at(aEventType).emplace_back(aAudioName);
}

bool AudioSource::Serialize(nl::json& outJsonObject)
{
    outJsonObject;
    return false;
}

bool AudioSource::Deserialize(nl::json& aJsonObject)
{
    std::string audioName = "";
    bool isOneShot = true;
    AudioSource::SourceType sourceType = AudioSource::SourceType::Non3D;
    bool playOnStart = false;

    if (aJsonObject.contains("AudioName"))
    {
        audioName = aJsonObject["AudioName"].get<std::string>();
    }

    if (aJsonObject.contains("IsOneShot"))
    {
        isOneShot = aJsonObject["IsOneShot"].get<bool>();
    }

    if (aJsonObject.contains("SourceType"))
    {
        sourceType = static_cast<AudioSource::SourceType>(aJsonObject["SourceType"].get<int>());
    }

    if (aJsonObject.contains("PlayOnStart"))
    {
        playOnStart = aJsonObject["PlayOnStart"].get<bool>();
    }

    AddAudioInstance(audioName, isOneShot, sourceType, playOnStart);

    if (aJsonObject.contains("PlayOnEvent"))
    {
        int eventInt = aJsonObject["PlayOnEvent"].get<int>();

        if (eventInt > 0 && eventInt < static_cast<int>(GameObjectEventType::Count))
        {
            SetAudioPlayOnEvent(audioName, static_cast<GameObjectEventType>(eventInt));
        }
    }

    return true;
}

void AudioSource::Update3DLocation(std::shared_ptr<AudioInstance> aInstance)
{
    std::shared_ptr<Transform> transform = gameObject->GetComponent<Transform>();
    if (transform)
    {
        aInstance->Set3dPosition(transform->GetTranslation(true), { 0, 0, 0 }, transform->GetForwardVector(true), transform->GetUpVector(true));
    }
}