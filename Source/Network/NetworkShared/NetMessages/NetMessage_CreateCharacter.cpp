#include "NetMessage_CreateCharacter.h"

NetMessage_CreateCharacter::NetMessage_CreateCharacter()
{
	myMessageType = NetMessageType::CreateCharacter;
}

void NetMessage_CreateCharacter::Serialize(NetBuffer& aBuffer)
{
	GuaranteedNetMessage::Serialize(aBuffer);
	aBuffer.WriteData(myNetworkID);
	aBuffer.WriteData(myIsPlayerCharacter);
	aBuffer.WriteData(myIsControlledByClient);
	aBuffer.WriteData(myPosition.x);
	aBuffer.WriteData(myPosition.y);
	aBuffer.WriteData(myPosition.z);
}

void NetMessage_CreateCharacter::Deserialize(NetBuffer& aBuffer)
{
	GuaranteedNetMessage::Deserialize(aBuffer);
	aBuffer.ReadData(myNetworkID);
	aBuffer.ReadData(myIsPlayerCharacter);
	aBuffer.ReadData(myIsControlledByClient);
	aBuffer.ReadData(myPosition.x);
	aBuffer.ReadData(myPosition.y);
	aBuffer.ReadData(myPosition.z);
}