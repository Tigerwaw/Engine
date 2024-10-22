#pragma once
#include "ControllerBase.h"
#include "WorldInterfacing/AI/I_AIEventSubscriber.hpp"

class AIEventsController : public ControllerBase, public I_AIEventSubscriber
{
public:
	~AIEventsController() override;
	void Start() override;
	CU::Vector3f GetDirection(CU::Vector3f aCurrentPosition) override;
	void Receive(const AIEvent& aEvent) override;

private:
	bool myIsPlayerHacking;
	CU::Vector3f myPlayerPosition;
};