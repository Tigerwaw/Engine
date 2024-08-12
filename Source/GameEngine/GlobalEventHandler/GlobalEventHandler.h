#pragma once
#include "GlobalEventSubscriber.hpp"
#include "GlobalEventType.h"

struct GlobalEvent
{
	GlobalEventType type = GlobalEventType::None;
	std::any data = 0;

	GlobalEvent() = default;
	GlobalEvent(GlobalEventType aType, std::any aData)
	{
		type = aType;
		data = aData;
	}
};

class GlobalEventHandler
{
public:
	GlobalEventHandler();
	~GlobalEventHandler();
	void Subscribe(const GlobalEventType& anEventType, GlobalEventSubscriber* aSubscriber);
	void Unsubscribe(const GlobalEventType& anEventType, GlobalEventSubscriber* aSubscriber);
	void Notify(const GlobalEvent aEvent);
	void Notify(const GlobalEventType aType, const std::any aData = 0);
private:
	std::vector<std::vector<GlobalEventSubscriber*>> mySubscribers;
};

