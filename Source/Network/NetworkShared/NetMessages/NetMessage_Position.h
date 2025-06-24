#pragma once
#include "NetworkEngine/BaseNetMessages/NetMessage.h"
#include <tuple>
#include <Math/Vector3.hpp>


class NetMessage_Position : public NetMessage
{
public:
	NetMessage_Position();
	void SetNetworkID(const unsigned aNetworkID) { myNetworkID = aNetworkID; }
	void SetPosition(const Math::Vector3f aPosition) { myPosition = aPosition; }
	void SetTimestamp(double aTimestamp) { myTimestamp = aTimestamp; }
	const unsigned GetNetworkID() const { return myNetworkID; }
	const Math::Vector3f GetPosition() const { return myPosition; }
	const double GetTimestamp() const { return myTimestamp; }

	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;
protected:
	unsigned myNetworkID;
	Math::Vector3f myPosition;
	double myTimestamp;
};