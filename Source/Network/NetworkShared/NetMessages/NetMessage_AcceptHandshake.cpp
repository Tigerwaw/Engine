#include "NetMessage_AcceptHandshake.h"

NetMessage_AcceptHandshake::NetMessage_AcceptHandshake()
{
	myMessageType = NetMessageType::AcceptHandshake;
}

void NetMessage_AcceptHandshake::Serialize(NetBuffer& aBuffer)
{
	NetMessage::Serialize(aBuffer);
}

void NetMessage_AcceptHandshake::Deserialize(NetBuffer& aBuffer)
{
	NetMessage::Deserialize(aBuffer);
}