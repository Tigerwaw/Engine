#include "NetMessage_CreateCharacter.h"

NetMessage_CreateCharacter::NetMessage_CreateCharacter()
{
	myMessageType = NetMessageType::Text;
}

void NetMessage_CreateCharacter::Serialize(NetBuffer& aBuffer)
{
	NetMessage::Serialize(aBuffer);
	aBuffer.WriteData(myPosition.x);
	aBuffer.WriteData(myPosition.y);
}

void NetMessage_CreateCharacter::Deserialize(NetBuffer& aBuffer)
{
	NetMessage::Deserialize(aBuffer);
	char buff[DEFAULT_BUFLEN]{ 0 };
	aBuffer.ReadData(buff);

	myPosition.x = buff[0];
	myPosition.y = buff[sizeof(float)];
	myPosition.z = buff[sizeof(float) * 2];
}

void NetMessage_CreateCharacter::GetStringRepresentation(char* outString, int aBufferSize) const
{
	outString;
	aBufferSize;
	//char buff[sizeof(CU::Vector3f)]{0};
	//buff[0] = myPosition.x;
	//buff[sizeof(float)] = myPosition.y;
	//memcpy_s(outString, aBufferSize, buff, sizeof(CU::Vector3f));
}
