#include "Enginepch.h"
#include "WindowsEventHandler.h"

WindowsEventHandler::WindowsEventHandler()
{
}

WindowsEventHandler::~WindowsEventHandler()
{
}

void WindowsEventHandler::AddEvent(WindowsEvent aEventType, std::function<void(MSG)> aFunction)
{
	unsigned eventType = static_cast<unsigned>(aEventType);
	if (myEvents.find(eventType) == myEvents.end())
	{
		myEvents.emplace(eventType, std::vector<std::function<void(MSG)>>());
	}

	myEvents.at(eventType).emplace_back(aFunction);
}

void WindowsEventHandler::HandleMessage(MSG* aMSG)
{
	MSG msg = *aMSG;

	unsigned eventType = static_cast<unsigned>(msg.message);

	if (myEvents.find(eventType) == myEvents.end()) return;

	for (auto& eventCallback : myEvents.at(eventType))
	{
		eventCallback(msg);
	}
}