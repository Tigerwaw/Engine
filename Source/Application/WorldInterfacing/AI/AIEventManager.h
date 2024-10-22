#pragma once
#include "AIEvent.hpp"
#include "I_AIEventSubscriber.hpp"

class AIEventManager
{
public:
	static AIEventManager& Get()
	{
		static AIEventManager instance;
		return instance;
	}
private:
	AIEventManager();
	~AIEventManager();
	AIEventManager(AIEventManager const&) = delete;
	void operator=(AIEventManager const&) = delete;
	static AIEventManager* myInstance;

public:
	//void Update();

	void Subscribe(const std::string& anEventType, I_AIEventSubscriber* aSubscriber);
	void Unsubscribe(const std::string& anEventType, I_AIEventSubscriber* aSubscriber);
	void Notify(const AIEvent aEvent);
	void Notify(const std::string aType, const std::any aData = 0);

private:
	std::vector<std::vector<I_AIEventSubscriber*>> mySubscribers;
	std::unordered_map<std::string, unsigned> myEventNameToIndex;
};