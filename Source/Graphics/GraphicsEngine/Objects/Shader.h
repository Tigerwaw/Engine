#pragma once
#include <wrl.h>
#include "GraphicsSettings.hpp"

struct ID3D11DeviceChild;

class Shader
{
	friend class RenderHardwareInterface;

public:
	Shader();
	~Shader();
	FORCEINLINE ShaderType GetShaderType() const { return myType; }
private:
	ShaderType myType = ShaderType::Unknown;
	Microsoft::WRL::ComPtr<ID3D11DeviceChild> myShader;
};

