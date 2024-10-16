#pragma once
#include "ControllerBase.h"

class AIEventsController : public ControllerBase
{
public:
	void Start() override;
	CU::Vector3f GetDirection(CU::Vector3f aCurrentPosition) override;
};