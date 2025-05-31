#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "Math/Vector3.hpp"


class RandomDirectionMovement : public Component
{
public:
	void Start() override;
	void Update() override;

	void SetDirection(Math::Vector3f aNewDirection);
	const Math::Vector3f& GetDirection() const;

private:
	Math::Vector3f myDirection;
	float mySpeed = 150.0f;
};

