#pragma once
#include "ComponentSystem/Component.h"
#include "Math/Vector.hpp"

namespace CU = CommonUtilities;

class MoveBetweenPoints :public Component
{
public:
	MoveBetweenPoints() = default;
	MoveBetweenPoints(std::vector<CU::Vector3f> aPointList, float aMoveSpeed);

	void SetPointList(std::vector<CU::Vector3f> aPointList);
	void SetMoveSpeed(float aMoveSpeed);

	void Start() override;
	void Update() override;

	bool Serialize(nl::json& outJsonObject) override;
	bool Deserialize(nl::json& aJsonObject) override;

private:
	float myMoveSpeed = 1.0f;
	std::vector<CU::Vector3f> myPointList;
	int myLastGoal = 0;
	int myCurrentGoal = 1;
	float myCurrentTimeMoved = 0;
	float myTimeToNextGoal = 1.0f;
};

