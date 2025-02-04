#include "NetMessage.h"

NetMessage::NetMessage()
{
	myMessageType = NetMessageType::Connect;
}

NetMessage::NetMessage(NetMessageType aType)
{
	myMessageType = aType;
}

void NetMessage::Serialize(NetBuffer& aBuffer)
{
	aBuffer.WriteData(myMessageType);
}

void NetMessage::Deserialize(NetBuffer& aBuffer)
{
	//aBuffer.ReadData(myMessageType);
	myMessageType = static_cast<NetMessageType>(aBuffer.GetBuffer()[0]);
}