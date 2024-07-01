#pragma once
#include <wrl.h>

struct ID3D11DeviceChild;

enum class ShaderType : unsigned
{
	Unknown,
	VertexShader,
	GeometryShader,
	PixelShader
};

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

