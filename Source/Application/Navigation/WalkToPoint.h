#pragma once
#include "GameEngine\ComponentSystem\Component.h"
#include "Math/Vector.hpp"


class WalkToPoint : public Component
{
public:
	WalkToPoint() = default;
	WalkToPoint(float aMoveSpeed);

	void SetMoveSpeed(float aMoveSpeed);
	void SetTarget(const Math::Vector3f& aPosition);

	void Start() override;
	void Update() override;

private:
	float myMoveSpeed = 1.0f;
	Math::Vector3f myTargetPosition;
};

