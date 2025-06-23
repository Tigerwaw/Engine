#pragma once
#include "NetworkEngine/GuaranteedNetMessage.h"
#include <tuple>
#include <Math/Vector3.hpp>


class NetMessage_RemoveCharacter : public GuaranteedNetMessage
{
public:
	NetMessage_RemoveCharacter();
	void SetNetworkID(const unsigned aNetworkID) { myNetworkID = aNetworkID; }
	const unsigned GetNetworkID() const { return myNetworkID; }

	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;

protected:
	unsigned myNetworkID;
};