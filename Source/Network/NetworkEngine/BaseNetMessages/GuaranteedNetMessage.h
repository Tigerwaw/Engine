#pragma once
#include "NetMessage.h"

class GuaranteedNetMessage : public NetMessage
{
public:
	void SetGuaranteedMessageID(int aGuaranteedMessageID) { myGuaranteedMessageID = aGuaranteedMessageID; }
	int GetGuaranteedMessageID() const { return myGuaranteedMessageID; }

	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;

private:
	int myGuaranteedMessageID;
};