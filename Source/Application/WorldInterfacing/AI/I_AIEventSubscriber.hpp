#pragma once

struct AIEvent;

class I_AIEventSubscriber
{
public:
	I_AIEventSubscriber() {}
	virtual ~I_AIEventSubscriber() {}
	virtual void Receive(const AIEvent& aEvent) = 0;
};