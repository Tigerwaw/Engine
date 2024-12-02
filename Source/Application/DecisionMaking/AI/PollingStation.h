#pragma once
#include "Math/Vector.hpp"
namespace CU = CommonUtilities;

class PollingStation
{
public:
	static PollingStation& Get()
	{
		static PollingStation instance;
		return instance;
	}
private:
	PollingStation();
	~PollingStation();
	PollingStation(PollingStation const&) = delete;
	void operator=(PollingStation const&) = delete;
	static PollingStation* myInstance;

public:
	void Update();
	void SetAIOne(std::shared_ptr<GameObject> aGameObject) { myAIOne = aGameObject; }
	void SetAITwo(std::shared_ptr<GameObject> aGameObject) { myAITwo = aGameObject; }
	void SetHealingWell(std::shared_ptr<GameObject> aGameObject) { myHealingWell = aGameObject; }
	void AddWall(std::shared_ptr<GameObject> aGameObject) { myWalls.emplace_back(aGameObject); }

	const std::shared_ptr<GameObject>& GetAIOne() { return myAIOne; }
	const CU::Vector3f GetAIOnePosition();
	const std::shared_ptr<GameObject>& GetAITwo() { return myAITwo; }
	const CU::Vector3f GetAITwoPosition();
	const CU::Vector3f GetHealingWellPosition();
	const std::vector<std::shared_ptr<GameObject>>& GetWalls() { return myWalls; }
	const std::vector<CU::Vector3f> GetWallPositions() const;
private:
	std::vector<std::shared_ptr<GameObject>> myWalls;
	std::shared_ptr<GameObject> myAIOne;
	std::shared_ptr<GameObject> myAITwo;
	std::shared_ptr<GameObject> myHealingWell;
};