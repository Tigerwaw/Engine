#include "GuaranteedNetMessage.h"

void GuaranteedNetMessage::Serialize(NetBuffer& aBuffer)
{
	NetMessage::Serialize(aBuffer);
	aBuffer.WriteData(myGuaranteedMessageID);
}

void GuaranteedNetMessage::Deserialize(NetBuffer& aBuffer)
{
	NetMessage::Deserialize(aBuffer);
	aBuffer.ReadData(myGuaranteedMessageID);
}
