#pragma once
#include "RHIStructs.h"

struct PipelineStateObject;
class ConstantBuffer;
class Shader;
class Texture;
class DynamicVertexBuffer;
struct ShaderInfo;

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
struct D3D11_BLEND_DESC;
struct D3D11_DEPTH_STENCIL_DESC;

class RenderHardwareInterface
{
public:
	RenderHardwareInterface();
	~RenderHardwareInterface();
	bool Initialize(HWND aWindowHandle, bool aEnableDebug);
#ifndef _RETAIL
	bool InitializeImGui();
#endif
	void Present() const;

	void SetResolution(float aNewWidth, float aNewHeight);
	void SetWindowSize(float aNewWidth, float aNewHeight);
	void MaximizeWindowSize();

	template <typename VertexType>
	bool CreateVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer) const;
	template <typename VertexType>
	bool CreateDynamicVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer, size_t aMaxVertexCount) const;
	bool CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer, bool aIsDynamic = false);
	bool CreateConstantBuffer(std::string_view aName, size_t aSize, unsigned aSlot, unsigned aPipelineStages, ConstantBuffer& outBuffer);

	void SetVertexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aVertexBuffer, size_t aVertexSize, size_t aVertexOffset = 0) const;
	void SetVertexBuffers(const std::vector<ID3D11Buffer*>& aVertexBuffers, const std::vector<unsigned>& aStrides, const std::vector<unsigned>& aOffsets) const;
	void SetIndexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aIndexBuffer) const;
	void SetConstantBuffer(const ConstantBuffer& aBuffer);

	template <typename VertexType>
	bool UpdateDynamicVertexBuffer(const std::vector<VertexType>& aVertexList, DynamicVertexBuffer& outVxBuffer) const;
	bool UpdateDynamicIndexBuffer(const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer);

	template<typename BufferData>
	bool UpdateConstantBuffer(const ConstantBuffer& aBuffer, const BufferData& aBufferData);

	void SetPrimitiveTopology(Topology aTopology) const;

	bool CreateInputLayout(std::string_view aName, Microsoft::WRL::ComPtr<ID3D11InputLayout>& outInputLayout, const std::vector<VertexElementDesc>& aInputLayoutDefinition, const uint8_t* aShaderDataPtr, size_t aShaderDataSize);
	bool CreateInputLayout(std::string_view aName, Microsoft::WRL::ComPtr<ID3D11InputLayout>& outInputLayout, const std::vector<VertexElementDesc>& aInputLayoutDefinition, const std::filesystem::path& aFilePath);
	void SetInputLayout(const Microsoft::WRL::ComPtr<ID3D11InputLayout>& aInputLayout);

	bool LoadShaderFromMemory(std::string_view aName, Shader& outShader, const uint8_t* aShaderDataPtr, size_t aShaderDataSize);
	bool LoadShaderFromFilePath(std::string_view aName, Shader& outShader, std::wstring aFilePath);

	void Draw(unsigned aVertexCount);
	void DrawIndexed(unsigned aStartIndex, unsigned aIndexCount) const;
	void DrawIndexedInstanced(unsigned aIndexCount, unsigned aInstanceCount, unsigned aStartIndex, unsigned aStartVertex, unsigned aStartInstance) const;

	void ChangePipelineState(const PipelineStateObject& aCurrentPSO, const PipelineStateObject& aNewPSO);

	bool CreateSamplerState(std::string_view aName, const D3D11_SAMPLER_DESC& aSamplerDesc);
	const Microsoft::WRL::ComPtr<ID3D11SamplerState>& GetSamplerState(const std::string& aName) const;

	bool CreateRasterizerState(std::string_view aName, const D3D11_RASTERIZER_DESC& aRasterDesc, PipelineStateObject& aPSO);
	bool CreateBlendState(std::string_view aName, const D3D11_BLEND_DESC& aBlendDesc, PipelineStateObject& aPSO);
	bool CreateDepthStencilState(std::string_view aName, const D3D11_DEPTH_STENCIL_DESC& aDepthStencilDesc, PipelineStateObject& aPSO);

	bool LoadTexture(std::string_view aName, const uint8_t* aTextureDataPtr, size_t aTextureDataSize, Texture& outTexture) const;
	bool SetTextureResource(unsigned aPipelineStages, unsigned aSlot, const Texture& aTexture) const;
	bool ClearTextureResourceSlot(unsigned aPipelineStages, unsigned aSlot) const;

	bool CreateTexture(std::string_view aName, unsigned aWidth, unsigned aHeight,
		RHITextureFormat aFormat, Texture& outTexture, bool aStaging, bool aShaderResource, bool aRenderTarget,
		bool aCpuAccessRead, bool aCpuAccessWrite) const;
	bool CreateShadowMap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture);
	bool CreateShadowCubemap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture);
	bool CreateLUT(std::string_view aName, unsigned aWidth, unsigned aHeight, std::shared_ptr<Texture> outTexture, const std::filesystem::path& aVSPath, const std::filesystem::path& aPSPath);

	void SetRenderTarget(std::shared_ptr<Texture> aRenderTarget, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget = true, bool aClearDepthStencil = true);
	void SetRenderTargets(const std::vector<std::shared_ptr<Texture>>& aRenderTargets, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget = true, bool aClearDepthStencil = true);

	void BeginEvent(std::string_view aEvent) const;
	void EndEvent() const;
	void SetMarker(std::string_view aMarker) const;

	ShaderInfo GetShaderInfo(const uint8_t* aTextureDataPtr, size_t aTextureDataSize);
	ShaderInfo GetShaderInfo(const std::filesystem::path& aShaderFilePath);

	bool HasInputLayout(const std::filesystem::path& aVSshaderPath) const;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> GetInputLayout(const std::filesystem::path& aVSshaderPath);

	FORCEINLINE std::shared_ptr<Texture> GetBackBuffer() { return myBackBuffer; }
	FORCEINLINE std::shared_ptr<Texture> GetDepthBuffer() { return myDepthBuffer; }
	FORCEINLINE std::shared_ptr<Texture> GetIntermediateTexture(IntermediateTexture aIntermediateTexture) { return myIntermediateTextures[aIntermediateTexture]; }

private:
	bool CreateVertexBufferInternal(std::string_view aName, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer, const uint8_t* aVertexDataPointer, size_t aNumVertices, size_t aVertexSize, bool aIsDynamic = false) const;
	bool UpdateDynamicVertexBufferInternal(DynamicVertexBuffer& outVxBuffer, const uint8_t* aVertexDataPointer, size_t aNumVertices, size_t aVertexSize) const;
	bool UpdateConstantBufferInternal(const ConstantBuffer& aBuffer, const void* aBufferData, size_t aBufferDataSize);

	void SetObjectName(Microsoft::WRL::ComPtr<ID3D11DeviceChild> aObject, std::string_view aName) const;

	void CreateDefaultSamplerStates();
	void CreateIntermediateTextures(unsigned aClientWidth, unsigned aClientHeight);

	Microsoft::WRL::ComPtr<ID3D11Device> myDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> myContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mySwapChain;
	Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> myUDA;

	std::shared_ptr<Texture> myBackBuffer;
	std::shared_ptr<Texture> myDepthBuffer;
	std::unordered_map<IntermediateTexture, std::shared_ptr<Texture>> myIntermediateTextures;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> mySamplerStates;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11InputLayout>> myInputLayouts;
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