#pragma once
#include "NetworkEngine/NetMessage.h"

class NetMessage_Test : public NetMessage
{
public:
	NetMessage_Test();
	void SetNetworkID(const unsigned aNetworkID) { myNetworkID = aNetworkID; }
	void SetInt(const int aInt) { myInt = aInt; }
	const unsigned GetNetworkID() const { return myNetworkID; }
	const int GetInt() const { return myInt; }

	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;
protected:
	unsigned myNetworkID;
	char myData[64];
	int myInt = 0;
};