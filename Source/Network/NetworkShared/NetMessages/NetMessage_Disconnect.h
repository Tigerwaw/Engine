#pragma once
#include "NetworkEngine/NetMessage.h"
#include <string>

class NetMessage_Disconnect : public NetMessage
{
public:
	NetMessage_Disconnect();
	void SetData(std::string aString) { myUsername = aString; }
	const std::string GetData() { return myUsername; }

	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;

	void GetStringRepresentation(char* outString, int aBufferSize) const override;

protected:
	std::string myUsername;
};

