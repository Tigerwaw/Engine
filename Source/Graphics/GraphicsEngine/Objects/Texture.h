#pragma once
#include <d3d11.h>

class RenderHardwareInterface;

#pragma warning( disable : 4471)
enum D3D11_USAGE;

class Texture
{
	friend class RenderHardwareInterface;

public:
	Texture();
	~Texture();
	ID3D11ShaderResourceView* GetSRV(); // Pretty much only used for displaying a texture in Dear ImGui
private:
	void SetProperties(UINT aArraySize, D3D11_USAGE aUsage, UINT aBindFlags, UINT aCPUAccessFlags);

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> myRTV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> myDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mySRV;

	std::array<float, 4> myClearColor = { 0, 0, 0, 0 };
	std::array<float, 6> myViewport = { 0, 0, 0, 0, 0, 0 };

	UINT myArraySize = 0;
	D3D11_USAGE myUsage;
	UINT myBindFlags = 0;
	UINT myCPUAccessFlags = 0;
};

