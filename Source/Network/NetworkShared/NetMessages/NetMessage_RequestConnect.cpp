#include "NetMessage_RequestConnect.h"

NetMessage_RequestConnect::NetMessage_RequestConnect()
{
	myMessageType = NetMessageType::RequestConnect;
}

void NetMessage_RequestConnect::SetUsername(const std::string& aUsername)
{
	myUsername = aUsername;
}

const std::string& NetMessage_RequestConnect::GetUsername() const
{
	return myUsername;
}

void NetMessage_RequestConnect::Serialize(NetBuffer& aBuffer)
{
	NetMessage::Serialize(aBuffer);
	aBuffer.WriteData(*myUsername.data(), static_cast<int>(myUsername.size()));
}

void NetMessage_RequestConnect::Deserialize(NetBuffer& aBuffer)
{
	NetMessage::Deserialize(aBuffer);
	char buff[NetworkDefines::defaultBufferSize]{ 0 };
	aBuffer.ReadData(buff);
	myUsername.assign(buff);
}