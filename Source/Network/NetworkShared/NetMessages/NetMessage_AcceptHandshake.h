#pragma once
#include "NetworkEngine/BaseNetMessages/NetMessage.h"
#include <string>

class NetMessage_AcceptHandshake : public NetMessage
{
public:
	NetMessage_AcceptHandshake();
	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;
};