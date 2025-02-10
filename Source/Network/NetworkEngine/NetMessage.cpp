#include "NetMessage.h"

NetMessage::NetMessage()
{
	myMessageType = NetMessageType::None;
}

NetMessage::NetMessage(NetMessageType aType)
{
	myMessageType = aType;
}

NetMessage::~NetMessage()
{

}

void NetMessage::Serialize(NetBuffer& aBuffer)
{
	aBuffer.WriteData(myMessageType);
}

void NetMessage::Deserialize(NetBuffer& aBuffer)
{
	aBuffer.ReadData(myMessageType);
}