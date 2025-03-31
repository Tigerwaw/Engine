#pragma once
#include "NetworkEngine/NetMessage.h"
#include <tuple>
#include <Math/Vector3.hpp>
namespace CU = CommonUtilities;

class NetMessage_RemoveCharacter : public NetMessage
{
public:
	NetMessage_RemoveCharacter();
	void SetNetworkID(const unsigned aNetworkID) { myNetworkID = aNetworkID; }
	const unsigned GetNetworkID() const { return myNetworkID; }

	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;

	void GetStringRepresentation(char* outString, int aBufferSize) const override;

protected:
	unsigned myNetworkID;
};