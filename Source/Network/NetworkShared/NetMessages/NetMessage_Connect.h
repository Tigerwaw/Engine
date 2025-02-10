#pragma once
#include "NetworkEngine/NetMessage.h"
#include <string>

class NetMessage_Connect : public NetMessage
{
public:
	NetMessage_Connect();
	void SetNetworkID(const unsigned aNetworkID) { myNetworkID = aNetworkID; }
	void SetUsername(const std::string& aUsername) { myUsername = aUsername; }
	const std::string GetUsername() const { return myUsername; }
	const unsigned GetNetworkID() const { return myNetworkID; }

	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;

	void GetStringRepresentation(char* outString, int aBufferSize) const override;

protected:
	unsigned myNetworkID;
	std::string myUsername;
};

