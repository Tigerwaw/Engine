#pragma once
#include <wrl.h>
#include <array>

class RenderHardwareInterface;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11ShaderResourceView;

#pragma warning( disable : 4471)
enum D3D11_USAGE;

class Texture
{
	friend class RenderHardwareInterface;

public:
	Texture();
	~Texture();
private:
	void SetProperties(UINT aArraySize, D3D11_USAGE aUsage, UINT aBindFlags, UINT aCPUAccessFlags);

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> myRTV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> myDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mySRV;

	std::array<float, 4> myClearColor = { 0, 0, 0, 0 };
	std::array<float, 6> myViewport = { 0, 0, 0, 0, 0, 0 };

	UINT myArraySize;
	D3D11_USAGE myUsage;
	UINT myBindFlags;
	UINT myCPUAccessFlags;
};

