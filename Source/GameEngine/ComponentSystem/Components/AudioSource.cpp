#include "AudioSource.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/Audio/AudioInstance.h"

void AudioSource::Start()
{
}

void AudioSource::Update()
{
}

void AudioSource::ReceiveEvent(const GameObjectEvent& aEvent)
{
    if (myPlayOnEvents.find(aEvent.type) == myPlayOnEvents.end()) return;

    for (auto& audioString : myPlayOnEvents.at(aEvent.type))
    {
        myAudioInstances[audioString]->Play();
    }
}

void AudioSource::AddAudioInstance(std::string aEventName)
{
    std::shared_ptr<AudioInstance> newAudioInstance = std::make_shared<AudioInstance>();
    if (!newAudioInstance->Initialize(aEventName)) return;

    myAudioInstances.emplace(aEventName, newAudioInstance);
}

void AudioSource::AddAudioPlayOnEvent(std::string aAudioName, GameObjectEventType aEventType)
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