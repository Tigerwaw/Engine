#pragma once
#include "NetworkEngine/GuaranteedNetMessage.h"
#include <string>

class NetMessage_Disconnect : public GuaranteedNetMessage
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

