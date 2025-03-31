#pragma once
#include "NetworkEngine/NetMessage.h"
#include <string>

class NetMessage_Connect : public NetMessage
{
public:
	NetMessage_Connect();
	void SetUsername(const std::string& aUsername) { myUsername = aUsername; }
	const std::string GetUsername() const { return myUsername; }

	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;

	void GetStringRepresentation(char* outString, int aBufferSize) const override;

protected:
	std::string myUsername;
};

