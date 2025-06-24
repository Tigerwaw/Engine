#pragma once
#include "NetworkEngine/BaseNetMessages/NetMessage.h"
#include <string>

class NetMessage_RequestConnect : public NetMessage
{
public:
	NetMessage_RequestConnect();

	void SetUsername(const std::string& aUsername);
	const std::string& GetUsername() const;

	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;
protected:
	std::string myUsername;
};

