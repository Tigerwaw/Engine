#include "NetMessage_RequestConnect.h"

NetMessage_RequestConnect::NetMessage_RequestConnect()
{
	myMessageType = NetMessageType::RequestConnect;
}

void NetMessage_RequestConnect::SetUsername(const std::string& aUsername)
{
	myUsername = aUsername;
}

std::string NetMessage_RequestConnect::GetUsername() const
{
	return myUsername;
}

void NetMessage_RequestConnect::Serialize(NetBuffer& aBuffer)
{
	NetMessage::Serialize(aBuffer);
}

void NetMessage_RequestConnect::Deserialize(NetBuffer& aBuffer)
{
	NetMessage::Deserialize(aBuffer);
}

void NetMessage_RequestConnect::GetStringRepresentation(char*, int) const
{
}
