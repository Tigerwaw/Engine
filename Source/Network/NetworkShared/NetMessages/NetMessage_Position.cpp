#include "NetMessage_Position.h"

NetMessage_Position::NetMessage_Position()
{
	myMessageType = NetMessageType::Position;
}

void NetMessage_Position::Serialize(NetBuffer& aBuffer)
{
	NetMessage::Serialize(aBuffer);
	aBuffer.WriteData(myNetworkID);
	aBuffer.WriteData(myPosition.x);
	aBuffer.WriteData(myPosition.y);
	aBuffer.WriteData(myPosition.z);
	aBuffer.WriteData(myTimestamp);
}

void NetMessage_Position::Deserialize(NetBuffer& aBuffer)
{
	NetMessage::Deserialize(aBuffer);
	aBuffer.ReadData(myNetworkID);
	aBuffer.ReadData(myPosition.x);
	aBuffer.ReadData(myPosition.y);
	aBuffer.ReadData(myPosition.z);
	aBuffer.ReadData(myTimestamp);
}