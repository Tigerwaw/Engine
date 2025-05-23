#include "NetMessage_RequestHandshake.h"

NetMessage_RequestHandshake::NetMessage_RequestHandshake()
{
	myMessageType = NetMessageType::RequestHandshake;
}

void NetMessage_RequestHandshake::Serialize(NetBuffer& aBuffer)
{
	NetMessage::Serialize(aBuffer);
}

void NetMessage_RequestHandshake::Deserialize(NetBuffer& aBuffer)
{
	NetMessage::Deserialize(aBuffer);
}

void NetMessage_RequestHandshake::GetStringRepresentation(char*, int) const
{
}