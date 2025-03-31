#include "NetMessage_RemoveCharacter.h"

NetMessage_RemoveCharacter::NetMessage_RemoveCharacter()
{
	myMessageType = NetMessageType::RemoveCharacter;
}

void NetMessage_RemoveCharacter::Serialize(NetBuffer& aBuffer)
{
	NetMessage::Serialize(aBuffer);
	aBuffer.WriteData(myNetworkID);
}

void NetMessage_RemoveCharacter::Deserialize(NetBuffer& aBuffer)
{
	NetMessage::Deserialize(aBuffer);
	aBuffer.ReadData(myNetworkID);
}

void NetMessage_RemoveCharacter::GetStringRepresentation(char* outString, int aBufferSize) const
{
	outString;
	aBufferSize;
}
