#pragma once
#include "NetBuffer.h"
#include "NetMessageType.h"

class NetMessage
{
public:
	NetMessage();
	virtual ~NetMessage();
	NetMessage(NetMessageType aType);

	NetMessageType GetType() const { return myMessageType; }

	virtual void Serialize(NetBuffer& aBuffer);
	virtual void Deserialize(NetBuffer& aBuffer);

	virtual void GetStringRepresentation(char* outString, int aBufferSize) const = 0;

protected:
	NetMessageType myMessageType;
};

