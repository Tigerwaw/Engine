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

	const CU::Vector3f& GetPlayerPosition() const { return myPlayerPosition; }
	const bool& IsPlayerHackingComputer() const { return myIsPlayerHackingComputer; }
private:
	CU::Vector3f myPlayerPosition;
	bool myIsPlayerHackingComputer = false;
};