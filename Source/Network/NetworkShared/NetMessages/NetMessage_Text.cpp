#include "NetMessage_Text.h"

NetMessage_Text::NetMessage_Text()
{
	myMessageType = NetMessageType::Text;
}

void NetMessage_Text::Serialize(NetBuffer& aBuffer)
{
	NetMessage::Serialize(aBuffer);
	aBuffer.WriteData(*myData.data(), static_cast<int>(myData.size()));
}

void NetMessage_Text::Deserialize(NetBuffer& aBuffer)
{
	NetMessage::Deserialize(aBuffer);
	char buff[NetworkDefines::defaultBufferSize]{ 0 };
	aBuffer.ReadData(buff);
	myData.assign(buff);
}