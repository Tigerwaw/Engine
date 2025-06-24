#include "AckNetMessage.h"

AckNetMessage::AckNetMessage()
{
	myMessageType = NetMessageType::AckMessage;
}

void AckNetMessage::Serialize(NetBuffer& aBuffer)
{
	NetMessage::Serialize(aBuffer);
	aBuffer.WriteData(myGuaranteedMessageID);
}

void AckNetMessage::Deserialize(NetBuffer& aBuffer)
{
	NetMessage::Deserialize(aBuffer);
	aBuffer.ReadData(myGuaranteedMessageID);
}
