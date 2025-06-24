#pragma once
#include "NetworkEngine/BaseNetMessages/GuaranteedNetMessage.h"
#include <tuple>
#include <Math/Vector3.hpp>


class NetMessage_CreateCharacter : public GuaranteedNetMessage
{
public:
	NetMessage_CreateCharacter();
	void SetNetworkID(const unsigned aNetworkID) { myNetworkID = aNetworkID; }
	void SetIsPlayerCharacter(const bool aIsPlayerCharacter) { myIsPlayerCharacter = aIsPlayerCharacter; }
	void SetIsControlledByClient(const bool aControlledByClient) { myIsControlledByClient = aControlledByClient; }
	void SetStartingPosition(const Math::Vector3f& aPosition) { myPosition = aPosition; }
	const unsigned GetNetworkID() const { return myNetworkID; }
	const unsigned GetIsPlayerCharacter() const { return myIsPlayerCharacter; }
	const unsigned GetIsControlledByClient() const { return myIsControlledByClient; }
	const Math::Vector3f GetPosition() const { return myPosition; }

	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;
protected:
	unsigned myNetworkID;
	bool myIsPlayerCharacter;
	bool myIsControlledByClient;
	Math::Vector3f myPosition;
};