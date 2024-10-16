#pragma once
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

class ControllerBase
{
public:
	virtual ~ControllerBase();
	virtual void Start() = 0;
	virtual CU::Vector3f GetDirection(CU::Vector3f aCurrentPosition) = 0;
protected:
	CU::Vector3f myTargetPosition;
};