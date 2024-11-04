#pragma once
#include "GameEngine\ComponentSystem\Component.h"
#include "Math/Vector.hpp"
namespace CU = CommonUtilities;

class WalkToPoint : public Component
{
public:
	WalkToPoint() = default;
	WalkToPoint(float aMoveSpeed);

	void SetMoveSpeed(float aMoveSpeed);
	void SetTarget(const CU::Vector3f& aPosition);

	void Start() override;
	void Update() override;

private:
	float myMoveSpeed = 1.0f;
	CU::Vector3f myTargetPosition;
};

