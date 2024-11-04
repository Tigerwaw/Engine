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
	void AddWatchedActor(std::shared_ptr<GameObject> aGameObject);
	void SetWanderer(std::shared_ptr<GameObject> aGameObject);

	const std::vector<CU::Vector3f> GetOtherActorPositions() const;
	const CU::Vector3f GetWandererPosition() const;
private:
	std::vector<std::shared_ptr<GameObject>> myWatchedActors;
	std::shared_ptr<GameObject> myWanderer;
};