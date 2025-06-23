#include "NetMessage_RemoveCharacter.h"

NetMessage_RemoveCharacter::NetMessage_RemoveCharacter()
{
	myMessageType = NetMessageType::RemoveCharacter;
}

void NetMessage_RemoveCharacter::Serialize(NetBuffer& aBuffer)
{
	GuaranteedNetMessage::Serialize(aBuffer);
	aBuffer.WriteData(myNetworkID);
}

void NetMessage_RemoveCharacter::Deserialize(NetBuffer& aBuffer)
{
	GuaranteedNetMessage::Deserialize(aBuffer);
	aBuffer.ReadData(myNetworkID);
}