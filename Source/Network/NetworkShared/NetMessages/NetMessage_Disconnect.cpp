#include "NetMessage_Disconnect.h"

NetMessage_Disconnect::NetMessage_Disconnect()
{
	myMessageType = NetMessageType::Disconnect;
}

void NetMessage_Disconnect::Serialize(NetBuffer& aBuffer)
{
	NetMessage::Serialize(aBuffer);
	aBuffer.WriteData(*myUsername.data(), static_cast<int>(myUsername.size()));
}

void NetMessage_Disconnect::Deserialize(NetBuffer& aBuffer)
{
	NetMessage::Deserialize(aBuffer);
	char buff[DEFAULT_BUFLEN]{ 0 };
	aBuffer.ReadData(buff);
	myUsername = std::string(buff);
}

void NetMessage_Disconnect::GetStringRepresentation(char* outString, int aBufferSize) const
{
	memcpy_s(outString, aBufferSize, myUsername.data(), myUsername.size());
}
