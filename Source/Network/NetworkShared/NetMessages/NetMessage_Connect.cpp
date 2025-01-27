#include "NetMessage_Connect.h"

NetMessage_Connect::NetMessage_Connect()
{
	myMessageType = NetMessageType::Connect;
}

void NetMessage_Connect::Serialize(NetBuffer& aBuffer)
{
	NetMessage::Serialize(aBuffer);
	aBuffer.WriteData(*myUsername.data(), static_cast<int>(myUsername.size()));
}

void NetMessage_Connect::Deserialize(const NetBuffer& aBuffer)
{
	NetMessage::Deserialize(aBuffer);
	char buff[DEFAULT_BUFLEN]{ 0 };
	aBuffer.ReadData(buff);
	myUsername = std::string(buff);
}

void NetMessage_Connect::GetStringRepresentation(char* outString, int aBufferSize) const
{
	outString = const_cast<char*>(myUsername.data());
	aBufferSize = static_cast<int>(myUsername.size());
}
