#pragma once
#include "NetworkEngine/NetMessage.h"
#include <string>

class NetMessage_RequestConnect : public NetMessage
{
public:
	NetMessage_RequestConnect();

	void SetUsername(const std::string& aUsername);
	std::string GetUsername() const;

	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;

	void GetStringRepresentation(char* outString, int aBufferSize) const override;
protected:
	std::string myUsername;
};

