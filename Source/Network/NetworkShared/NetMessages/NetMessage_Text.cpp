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

void NetMessage_Text::Deserialize(const NetBuffer& aBuffer)
{
	NetMessage::Deserialize(aBuffer);
	char buff[DEFAULT_BUFLEN]{ 0 };
	aBuffer.ReadData(buff);
	myData = std::string(buff);
}

void NetMessage_Text::GetStringRepresentation(char* outString, int aBufferSize) const
{
	outString = const_cast<char*>(myData.data());
	aBufferSize = static_cast<int>(myData.size());
}
