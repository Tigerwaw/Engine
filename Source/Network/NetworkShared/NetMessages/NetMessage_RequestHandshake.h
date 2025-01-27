#pragma once
#include "../NetMessage.h"
#include <string>

class NetMessage_RequestHandshake : public NetMessage
{
public:
	NetMessage_RequestHandshake();
	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(const NetBuffer& aBuffer) override;

	void GetStringRepresentation(char* outString, int aBufferSize) const override;
};