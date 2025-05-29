#include "Enginepch.h"

#include "GlobalEventHandler.h"

GlobalEventHandler::GlobalEventHandler()
{
	for (int i = 0; i < static_cast<int>(GlobalEventType::Count); i++)
	{
		mySubscribers.emplace_back();
	}
}

GlobalEventHandler::~GlobalEventHandler()
{
	for (auto& subscriberVector : mySubscribers)
	{
		subscriberVector.clear();
	}
}

void GlobalEventHandler::Subscribe(const GlobalEventType& aEventType, GlobalEventSubscriber* aSubscriber)
{
	mySubscribers[static_cast<int>(aEventType)].push_back(aSubscriber);
}

void GlobalEventHandler::Unsubscribe(const GlobalEventType& aEventType, GlobalEventSubscriber* aSubscriber)
{
	
	auto& subs = mySubscribers[static_cast<int>(aEventType)];

	for (int i = 0; i < subs.size(); i++)
	{
		if (aSubscriber == subs[i])
		{
			subs.erase(subs.begin() + i);
			break;
		}
	}
}

void GlobalEventHandler::Notify(const GlobalEvent aGlobalEvent)
{
	for (int i = 0; i < mySubscribers[static_cast<int>(aGlobalEvent.type)].size(); i++)
	{
		mySubscribers[static_cast<int>(aGlobalEvent.type)][i]->Receive(aGlobalEvent);
	}
}

void GlobalEventHandler::Notify(const GlobalEventType aType, const std::any aData)
{
	GlobalEvent newGlobalEvent;
	newGlobalEvent.type = aType;
	newGlobalEvent.data = aData;
	Notify(newGlobalEvent);
}

