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
}

void NetMessage_Position::Deserialize(NetBuffer& aBuffer)
{
	NetMessage::Deserialize(aBuffer);
	aBuffer.ReadData(myNetworkID);
	aBuffer.ReadData(myPosition.x);
	aBuffer.ReadData(myPosition.y);
	aBuffer.ReadData(myPosition.z);
}

void NetMessage_Position::GetStringRepresentation(char* outString, int aBufferSize) const
{
	outString;
	aBufferSize;
	//char buff[sizeof(CU::Vector3f)]{0};
	//buff[0] = myPosition.x;
	//buff[sizeof(float)] = myPosition.y;
	//memcpy_s(outString, aBufferSize, buff, sizeof(CU::Vector3f));
}
