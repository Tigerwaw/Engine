#pragma once
#include "NetworkEngine/GuaranteedNetMessage.h"
#include <tuple>
#include <Math/Vector3.hpp>


class NetMessage_CreateCharacter : public GuaranteedNetMessage
{
public:
	NetMessage_CreateCharacter();
	void SetNetworkID(const unsigned aNetworkID) { myNetworkID = aNetworkID; }
	void SetStartingPosition(const Math::Vector3f& aPosition) { myPosition = aPosition; }
	const unsigned GetNetworkID() const { return myNetworkID; }
	const Math::Vector3f GetPosition() const { return myPosition; }

	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;
protected:
	unsigned myNetworkID;
	Math::Vector3f myPosition;
};