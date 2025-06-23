#pragma once
#include "NetworkEngine/NetMessage.h"
#include <string>

class NetMessage_RequestHandshake : public NetMessage
{
public:
	NetMessage_RequestHandshake();
	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;
};