#include "NetMessage_CreateCharacter.h"

NetMessage_CreateCharacter::NetMessage_CreateCharacter()
{
	myMessageType = NetMessageType::CreateCharacter;
}

void NetMessage_CreateCharacter::Serialize(NetBuffer& aBuffer)
{
	NetMessage::Serialize(aBuffer);
	aBuffer.WriteData(myNetworkID);
	aBuffer.WriteData(myPosition.x);
	aBuffer.WriteData(myPosition.y);
	aBuffer.WriteData(myPosition.z);
}

void NetMessage_CreateCharacter::Deserialize(NetBuffer& aBuffer)
{
	NetMessage::Deserialize(aBuffer);
	aBuffer.ReadData(myNetworkID);
	aBuffer.ReadData(myPosition.x);
	aBuffer.ReadData(myPosition.y);
	aBuffer.ReadData(myPosition.z);
}

void NetMessage_CreateCharacter::GetStringRepresentation(char* outString, int aBufferSize) const
{
	outString;
	aBufferSize;
}
