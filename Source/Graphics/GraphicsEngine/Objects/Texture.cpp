#include "GraphicsEngine.pch.h"
#include "Texture.h"

Texture::Texture() = default;
Texture::~Texture() = default;

void Texture::SetProperties(UINT aArraySize, D3D11_USAGE aUsage, UINT aBindFlags, UINT aCPUAccessFlags)
{
	myArraySize = aArraySize;
	myUsage = aUsage;
	myBindFlags = aBindFlags;
	myCPUAccessFlags = aCPUAccessFlags;
}

ID3D11ShaderResourceView* Texture::GetSRV()
{
	return mySRV.Get();
}