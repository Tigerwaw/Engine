#pragma once
#include "wrl.h"
#include <string>

struct ID3D11Buffer;

class ConstantBuffer
{
	friend class RenderHardwareInterface;

public:
	ConstantBuffer();
	~ConstantBuffer();
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> myBuffer;
	std::string myName;
	size_t mySize = 0;

	unsigned myPipelineStages = 0;
	unsigned mySlotIndex = 0;
};

