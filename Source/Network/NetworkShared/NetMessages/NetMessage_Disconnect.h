#pragma once
#include "NetworkEngine/BaseNetMessages/NetMessage.h"
#include <string>

class NetMessage_Disconnect : public NetMessage
{
public:
	NetMessage_Disconnect();
	void SetData(const std::string& aString) { myUsername = aString; }
	const std::string& GetData() { return myUsername; }

	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;
protected:
	std::string myUsername;
};

