#pragma once

struct GlobalEvent;

class GlobalEventSubscriber
{
public:
	GlobalEventSubscriber() {}
	virtual ~GlobalEventSubscriber() {}
	virtual void Receive(const GlobalEvent& aEvent) = 0;
};

