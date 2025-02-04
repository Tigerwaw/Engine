#pragma once
#include "NetworkEngine/NetMessage.h"
#include <string>

class NetMessage_Connect : public NetMessage
{
public:
	NetMessage_Connect();
	void SetData(const std::string& aString) { myUsername = aString; }
	const std::string& GetData() { return myUsername; }

	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;

	void GetStringRepresentation(char* outString, int aBufferSize) const override;

protected:
	std::string myUsername;
};

