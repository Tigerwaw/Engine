#pragma once
#include <wrl.h>
#include <memory>
#include <unordered_map>

#include "RHIStructs.h"

struct PipelineStateObject;
class ConstantBuffer;
class Shader;
class Texture;
class DynamicVertexBuffer;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11Buffer;
struct ID3D11InputLayout;
struct ID3DUserDefinedAnnotation;
struct ID3D11DeviceChild;
struct ID3D11SamplerState;
struct D3D11_SAMPLER_DESC;
struct D3D11_RASTERIZER_DESC;

class RenderHardwareInterface
{
public:
	RenderHardwareInterface();
	~RenderHardwareInterface();
	bool Initialize(HWND aWindowHandle, bool aEnableDebug);
#ifdef _DEBUG
	bool InitializeImGui();
#endif
	void Present() const;

	void SetResolution(float aNewWidth, float aNewHeight);
	void SetWindowSize(float aNewWidth, float aNewHeight);

	template <typename VertexType>
	bool CreateVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer) const;
	template <typename VertexType>
	bool CreateDynamicVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer, size_t aMaxVertexCount) const;
	bool CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer);
	bool CreateConstantBuffer(std::string_view aName, size_t aSize, unsigned aSlot, unsigned aPipelineStages, ConstantBuffer& outBuffer);

	void SetVertexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aVertexBuffer, size_t aVertexSize, size_t aVertexOffset = 0) const;
	void SetIndexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aIndexBuffer) const;
	void SetConstantBuffer(const ConstantBuffer& aBuffer);

	template <typename VertexType>
	bool UpdateDynamicVertexBuffer(const std::vector<VertexType>& aVertexList, DynamicVertexBuffer& outVxBuffer) const;

	template<typename BufferData>
	bool UpdateConstantBuffer(const ConstantBuffer& aBuffer, const BufferData& aBufferData);

	void SetPrimitiveTopology(Topology aTopology) const;

	bool CreateInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout>& outInputLayout, const std::vector<VertexElementDesc>& aInputLayoutDefinition, const uint8_t* aShaderDataPtr, size_t aShaderDataSize);
	bool CreateInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout>& outInputLayout, const std::vector<VertexElementDesc>& aInputLayoutDefinition, std::wstring aFilePath);
	void SetInputLayout(const Microsoft::WRL::ComPtr<ID3D11InputLayout>& aInputLayout);

	bool LoadShaderFromMemory(std::string_view aName, Shader& outShader, const uint8_t* aShaderDataPtr, size_t aShaderDataSize);
	bool LoadShaderFromFilePath(std::string_view aName, Shader& outShader, std::wstring aFilePath);

	void Draw(unsigned aVertexCount);
	void DrawIndexed(unsigned aStartIndex, unsigned aIndexCount) const;

	void ChangePipelineState(const PipelineStateObject& aNewPSO);

	void CreateDefaultSamplerStates();
	bool CreateSamplerState(std::string_view aName, const D3D11_SAMPLER_DESC& aSamplerDesc);
	const Microsoft::WRL::ComPtr<ID3D11SamplerState>& GetSamplerState(const std::string& aName) const;

	bool CreateRasterizerState(std::string_view aName, const D3D11_RASTERIZER_DESC& aRasterDesc, PipelineStateObject& aPSO);

	bool LoadTexture(std::string_view aName, const uint8_t* aTextureDataPtr, size_t aTextureDataSize, Texture& outTexture) const;
	bool SetTextureResource(unsigned aPipelineStages, unsigned aSlot, const Texture& aTexture) const;
	bool ClearTextureResourceSlot(unsigned aPipelineStages, unsigned aSlot) const;

	bool CreateTexture(std::string_view aName, unsigned aWidth, unsigned aHeight,
		RHITextureFormat aFormat, Texture& outTexture, bool aStaging, bool aShaderResource, bool aRenderTarget,
		bool aCpuAccessRead, bool aCpuAccessWrite) const;
	bool CreateShadowMap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture);
	bool CreateShadowCubemap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture);
	bool CreateLUT(std::string_view aName, unsigned aWidth, unsigned aHeight, std::shared_ptr<Texture> outTexture);

	void SetRenderTarget(std::shared_ptr<Texture> aRenderTarget, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget = true, bool aClearDepthStencil = true);

	void BeginEvent(std::string_view aEvent) const;
	void EndEvent() const;
	void SetMarker(std::string_view aMarker) const;

	FORCEINLINE std::shared_ptr<Texture> GetBackBuffer() { return myBackBuffer; }
	FORCEINLINE std::shared_ptr<Texture> GetDepthBuffer() { return myDepthBuffer; }

private:
	bool CreateVertexBufferInternal(std::string_view aName, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer, const uint8_t* aVertexDataPointer, size_t aNumVertices, size_t aVertexSize, bool aIsDynamic = false) const;
	bool UpdateDynamicVertexBufferInternal(DynamicVertexBuffer& outVxBuffer, const uint8_t* aVertexDataPointer, size_t aNumVertices, size_t aVertexSize) const;
	bool UpdateConstantBufferInternal(const ConstantBuffer& aBuffer, const void* aBufferData, size_t aBufferDataSize);

	void SetObjectName(Microsoft::WRL::ComPtr<ID3D11DeviceChild> aObject, std::string_view aName) const;

	Microsoft::WRL::ComPtr<ID3D11Device> myDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> myContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mySwapChain;
	Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> myUDA;

	std::shared_ptr<Texture> myBackBuffer;
	std::shared_ptr<Texture> myDepthBuffer;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> mySamplerStates;
};

template<typename VertexType>
bool RenderHardwareInterface::CreateVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer) const
{
	const size_t vxSize = sizeof(VertexType);
	const size_t vxCount = aVertexList.size();
	return CreateVertexBufferInternal(aName, outVxBuffer, reinterpret_cast<const uint8_t*>(aVertexList.data()), vxCount, vxSize);
}

template<typename VertexType>
inline bool RenderHardwareInterface::CreateDynamicVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer, size_t aMaxVertexCount) const
{
	const size_t vxSize = sizeof(VertexType);
	return CreateVertexBufferInternal(aName, outVxBuffer, reinterpret_cast<const uint8_t*>(aVertexList.data()), aMaxVertexCount, vxSize, true);
}

template<typename VertexType>
inline bool RenderHardwareInterface::UpdateDynamicVertexBuffer(const std::vector<VertexType>& aVertexList, DynamicVertexBuffer& outVxBuffer) const
{
	const size_t vxSize = sizeof(VertexType);
	const size_t vxCount = aVertexList.size();
	return UpdateDynamicVertexBufferInternal(outVxBuffer, reinterpret_cast<const uint8_t*>(aVertexList.data()), vxCount, vxSize);
}

template<typename BufferData>
bool RenderHardwareInterface::UpdateConstantBuffer(const ConstantBuffer& aBuffer, const BufferData& aBufferData)
{
	const size_t dataSize = sizeof(BufferData);

	return UpdateConstantBufferInternal(aBuffer, &aBufferData, dataSize);
}
