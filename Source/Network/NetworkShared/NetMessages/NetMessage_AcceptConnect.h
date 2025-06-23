#pragma once
#include "NetworkEngine/NetMessage.h"
#include <string>

class NetMessage_AcceptConnect : public NetMessage
{
public:
	NetMessage_AcceptConnect();

	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;
};

