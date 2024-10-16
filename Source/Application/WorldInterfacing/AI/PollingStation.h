#pragma once
#include "Math/Vector.hpp"
namespace CU = CommonUtilities;

class PollingStation
{
public:
	void Update();

	const CU::Vector3f& GetPlayerPosition();
	const CU::Vector3f& GetComputer1Position();
	const CU::Vector3f& GetComputer2Position();
	const CU::Vector3f& GetComputer3Position();
private:
	CU::Vector3f myPlayerPosition;
	CU::Vector3f myComputer1Position;
	CU::Vector3f myComputer2Position;
	CU::Vector3f myComputer3Position;
};