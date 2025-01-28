#include "NetMessage.h"

NetMessage::NetMessage()
{
	myMessageType = NetMessageType::Connect;
}

NetMessage::NetMessage(NetMessageType aType)
{
	myMessageType = aType;
}

void NetMessage::Serialize(NetBuffer& aBuffer)
{
	aBuffer.WriteData(static_cast<int>(myMessageType));
}

void NetMessage::Deserialize(const NetBuffer& aBuffer)
{
	myMessageType = static_cast<NetMessageType>(aBuffer.GetBuffer()[0]);
}