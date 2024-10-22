#include "Enginepch.h"
#include "AIEventManager.h"

AIEventManager::AIEventManager()
{
}

AIEventManager::~AIEventManager()
{
	for (auto& subscriberVector : mySubscribers)
	{
		subscriberVector.clear();
	}

	myEventNameToIndex.clear();
}

void AIEventManager::Subscribe(const std::string& anEventType, I_AIEventSubscriber* aSubscriber)
{
	if (!myEventNameToIndex.contains(anEventType))
	{
		mySubscribers.emplace_back(std::vector<I_AIEventSubscriber*>());
		myEventNameToIndex[anEventType] = static_cast<unsigned>(mySubscribers.size() - 1);
	}

	mySubscribers[myEventNameToIndex[anEventType]].emplace_back(aSubscriber);
}

void AIEventManager::Unsubscribe(const std::string& anEventType, I_AIEventSubscriber* aSubscriber)
{
	if (mySubscribers.empty()) return;
	if (myEventNameToIndex.empty()) return;
	if (!myEventNameToIndex.contains(anEventType)) return;

	std::vector<I_AIEventSubscriber*>& eventVector = mySubscribers[myEventNameToIndex[anEventType]];
	if (auto it = std::find(eventVector.begin(), eventVector.end(), aSubscriber); it != eventVector.end())
	{
		if (eventVector.size() > 1)
		{
			int index = static_cast<int>(it - eventVector.begin());
			std::swap(eventVector[index], eventVector.back());
			eventVector.pop_back();
		}
		else
		{
			eventVector.clear();
			mySubscribers.erase(mySubscribers.begin() + myEventNameToIndex[anEventType]);
			myEventNameToIndex.erase(anEventType);
		}

	}
}

void AIEventManager::Notify(const AIEvent aEvent)
{
	if (!myEventNameToIndex.contains(aEvent.type)) return;

	std::vector<I_AIEventSubscriber*>& eventVector = mySubscribers[myEventNameToIndex[aEvent.type]];
	for (auto& subscriber : eventVector)
	{
		subscriber->Receive(aEvent);
	}
}

void AIEventManager::Notify(const std::string aType, const std::any aData)
{
	AIEvent newAIEvent;
	newAIEvent.type = aType;
	newAIEvent.data = aData;
	Notify(newAIEvent);
}
