#pragma once
#include "NetBuffer.h"

enum class NetMessageType
{
	Connect,
	Disconnect,
	Text,
	HandshakeRequest,
	HandshakeAccept
};

class NetMessage
{
public:
	NetMessage();
	virtual ~NetMessage() {};
	NetMessage(NetMessageType aType);

	NetMessageType GetType() const { return myMessageType; }

	virtual void Serialize(NetBuffer& aBuffer);
	virtual void Deserialize(const NetBuffer& aBuffer);

	virtual void GetStringRepresentation(char* outString, int aBufferSize) const = 0;

protected:
	NetMessageType myMessageType;
};

