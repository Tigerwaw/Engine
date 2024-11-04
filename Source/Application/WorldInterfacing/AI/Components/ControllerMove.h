#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "Math/Vector.hpp"
namespace CU = CommonUtilities;

class ControllerBase;

class ControllerMove : public Component
{
public:
	enum class ControllerType
	{
		Wander,
		Seek,
		Separate
	};

	ControllerMove() = default;
	~ControllerMove();
	ControllerMove(float aMoveSpeed, ControllerType aControllerType);

	void SetMoveSpeed(float aMoveSpeed);
	void SetControllerType(ControllerType aControllerType);

	void Start() override;
	void Update() override;

private:
	ControllerBase* myController;
	float myMoveSpeed = 1.0f;
};