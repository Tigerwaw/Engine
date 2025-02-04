#pragma once
#include "NetworkEngine/NetMessage.h"
#include <Math/Vector3.hpp>
namespace CU = CommonUtilities;

class NetMessage_CreateCharacter : public NetMessage
{
public:
	NetMessage_CreateCharacter();
	void SetData(const CU::Vector3f& aPosition) { myPosition = aPosition; }
	const CU::Vector3f& GetData() { return myPosition; }

	void Serialize(NetBuffer& aBuffer) override;
	void Deserialize(NetBuffer& aBuffer) override;

	void GetStringRepresentation(char* outString, int aBufferSize) const override;

protected:
	CU::Vector3f myPosition;
};