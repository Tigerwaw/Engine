#include "NetMessage_Disconnect.h"

NetMessage_Disconnect::NetMessage_Disconnect()
{
	myMessageType = NetMessageType::Disconnect;
}

void NetMessage_Disconnect::Serialize(NetBuffer& aBuffer)
{
	GuaranteedNetMessage::Serialize(aBuffer);
	aBuffer.WriteData(*myUsername.data(), static_cast<int>(myUsername.size()));
}

void NetMessage_Disconnect::Deserialize(NetBuffer& aBuffer)
{
	GuaranteedNetMessage::Deserialize(aBuffer);
	char buff[DEFAULT_BUFLEN]{ 0 };
	aBuffer.ReadData(buff);
	myUsername = std::string(buff);
}