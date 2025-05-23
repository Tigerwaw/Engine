#include "NetMessage_AcceptConnect.h"

NetMessage_AcceptConnect::NetMessage_AcceptConnect()
{
	myMessageType = NetMessageType::ConnectAccept;
}

void NetMessage_AcceptConnect::Serialize(NetBuffer& aBuffer)
{
	NetMessage::Serialize(aBuffer);
}

void NetMessage_AcceptConnect::Deserialize(NetBuffer& aBuffer)
{
	NetMessage::Deserialize(aBuffer);
}

void NetMessage_AcceptConnect::GetStringRepresentation(char*, int) const
{
}
