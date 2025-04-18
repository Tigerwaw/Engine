#pragma once
#include "NetworkEngine/NetMessage.h"
#include <tuple>
#include <Math/Vector3.hpp>
namespace CU = CommonUtilities;

class NetMessage_CreateCharacter : public NetMessage
{
public:
	NetMessage_CreateCharacter();
	void SetNetworkID(const unsigned aNetworkID) { myNetworkID = aNetworkID; }
	void SetStartingPosition(const CU::Vector3f& aPosition) { myPosition = aPosition; }
	const unsigned GetNetworkID() const { return myNetworkID; }
	const CU::Vector3f GetPosition() const { return myPosition; }

	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;

	void GetStringRepresentation(char* outString, int aBufferSize) const override;

protected:
	unsigned myNetworkID;
	CU::Vector3f myPosition;
};