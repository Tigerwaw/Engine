#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "Math/Vector3.hpp"
namespace CU = CommonUtilities;

class RandomDirectionMovement : public Component
{
public:
	void Start() override;
	void Update() override;

	void SetDirection(CU::Vector3f aNewDirection);
	const CU::Vector3f& GetDirection() const;

private:
	CU::Vector3f myDirection;
	float mySpeed = 150.0f;
};

