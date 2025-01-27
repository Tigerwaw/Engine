#pragma once
#include "../NetMessage.h"
#include <string>

class NetMessage_Connect : public NetMessage
{
public:
	NetMessage_Connect();
	void SetData(std::string aString) { myUsername = aString; }
	const std::string& GetData() { return myUsername; }

	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(const NetBuffer& aBuffer) override;

	void GetStringRepresentation(char* outString, int aBufferSize) const override;

protected:
	std::string myUsername;
};

