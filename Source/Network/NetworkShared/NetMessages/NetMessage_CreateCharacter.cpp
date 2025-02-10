#include "NetMessage_CreateCharacter.h"

NetMessage_CreateCharacter::NetMessage_CreateCharacter()
{
	myMessageType = NetMessageType::CreateCharacter;
}

void NetMessage_CreateCharacter::Serialize(NetBuffer& aBuffer)
{
	NetMessage::Serialize(aBuffer);
	aBuffer.WriteData(myNetworkID);
	aBuffer.WriteData(myIsClient);
}

void NetMessage_CreateCharacter::Deserialize(NetBuffer& aBuffer)
{
	NetMessage::Deserialize(aBuffer);
	aBuffer.ReadData(myNetworkID);
	aBuffer.ReadData(myIsClient);
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
