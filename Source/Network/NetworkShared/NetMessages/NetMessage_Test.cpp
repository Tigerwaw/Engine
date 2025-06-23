#include "NetMessage_Test.h"

NetMessage_Test::NetMessage_Test()
{
	myMessageType = NetMessageType::Test;
}

void NetMessage_Test::Serialize(NetBuffer& aBuffer)
{
	NetMessage::Serialize(aBuffer);
	aBuffer.WriteData(myNetworkID);
	aBuffer.WriteData(myInt);
	aBuffer.WriteData(myData);
}

void NetMessage_Test::Deserialize(NetBuffer& aBuffer)
{
	NetMessage::Deserialize(aBuffer);
	aBuffer.ReadData(myNetworkID);
	aBuffer.ReadData(myInt);
	aBuffer.ReadData(myData);
}