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
	std::shared_ptr<GameObject> GetRandomAIActor(std::shared_ptr<GameObject> aSelf);
	void AddAIActor(std::shared_ptr<GameObject> aGameObject);
	void SetHealingWell(std::shared_ptr<GameObject> aGameObject) { myHealingWell = aGameObject; }
	void AddWall(std::shared_ptr<GameObject> aGameObject) { myWalls.emplace_back(aGameObject); }

	const CU::Vector3f GetHealingWellPosition();
	const std::vector<std::shared_ptr<GameObject>>& GetWalls() { return myWalls; }
	const std::vector<CU::Vector3f> GetWallPositions() const;
private:
	std::vector<std::shared_ptr<GameObject>> myWalls;
	std::vector<std::shared_ptr<GameObject>> myAIActors;
	std::shared_ptr<GameObject> myHealingWell;
};