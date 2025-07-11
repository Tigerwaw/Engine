#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <wrl.h>
#include "RHI/RenderHardwareInterface.h"
#include "Objects/PipelineStateObject.h"
#include "GraphicsCommands/GraphicsCommandList.h"
#include "Objects/ConstantBuffers/ConstantBuffer.h"
#include "PostProcessingSettings.h"
#include "Drawer.h"

class Shader;
class Mesh;
class Text;
class Sprite;
class DynamicVertexBuffer;
class GraphicsCommandList;
class GraphicsCommandBase;
class GBuffer;
class ParticleEmitter;
class TrailEmitter;

struct ID3D11InputLayout;

struct Vertex;

enum class ConstantBufferType
{
	ObjectBuffer,
	FrameBuffer,
	AnimationBuffer,
	MaterialBuffer,
	LightBuffer,
	ShadowBuffer,
	SpriteBuffer,
	PostProcessBuffer
};

enum class BlendMode
{
	None,
	Alpha,
	Additive,
	COUNT
};

struct PSODescription
{
	std::string name;
	std::vector<VertexElementDesc> inputLayoutDefinition;
	unsigned vertexStride;
	std::wstring vsPath;
	std::shared_ptr<Shader> vsShader;
	std::shared_ptr<Shader> gsShader;
	std::shared_ptr<Shader> psShader;
	std::unordered_map<unsigned, std::string> samplerList;
	unsigned fillMode = 3;
	unsigned cullMode = 3;
	bool antiAliasedLine = false;
	BlendMode blendMode = BlendMode::None;
	bool alphaToCoverage = false;
	bool independentBlend = false;
	bool useReadOnlyDepthStencilState = false;
};

class GraphicsEngine
{
	friend class Drawer;

public:
	static GraphicsEngine& Get()
	{
		static GraphicsEngine myInstance;
		return myInstance;
	}
	bool Initialize(HWND aWindowHandle, Math::Vector2f aResolution, const std::filesystem::path& aContentRoot);

#ifndef _RETAIL
	bool InitializeImGui();
#endif

	PostProcessingSettings& GetPostProcessingSettings() { return *myPostProcessingSettings; }
	Drawer& GetDrawer() { return *myDrawer; }

	void BeginFrame();
	void RenderFrame();
	void EndFrame();

	void SetResolution(float aNewWidth, float aNewHeight);
	void SetWindowSize(float aNewWidth, float aNewHeight);
	void MaximizeWindowSize();

	template<typename BufferData>
	bool UpdateAndSetConstantBuffer(ConstantBufferType aBufferType, const BufferData& aDataBlock);

	void ChangePipelineState(const std::shared_ptr<PipelineStateObject> aNewPSO);

	bool CreateTexture(std::string_view aName, unsigned aWidth, unsigned aHeight,
		RHITextureFormat aFormat, Texture& outTexture, bool aStaging, bool aShaderResource, bool aRenderTarget,
		bool aCpuAccessRead, bool aCpuAccessWrite) const;
	bool LoadTexture(std::string_view aName, const uint8_t* aTextureDataPtr, size_t aTextureDataSize, Texture& outTexture) const;
	bool LoadTexture(const std::filesystem::path& aFilePath, Texture& outTexture) const;
	bool SetTextureResource_PS(unsigned aSlot, Texture& aTexture);
	bool SetTextureResource_VS(unsigned aSlot, Texture& aTexture);
	bool ClearTextureResource_PS(unsigned aSlot);
	bool ClearTextureResource_VS(unsigned aSlot);

	bool LoadShader(const std::filesystem::path& aFilePath, Shader& outShader);
	bool CreatePSO(std::shared_ptr<PipelineStateObject> aPSO, PSODescription& aPSOdesc);
	std::shared_ptr<PipelineStateObject> GetDefaultPSO() { return myDefaultPSO; }

	const bool CompareShaderParameters(const std::filesystem::path& aShaderOnePath, const std::filesystem::path& aShaderTwoPath) const;

	const bool ValidateShaderCombination(
		const std::filesystem::path& aVertexShaderPath,
		const std::filesystem::path& aGeometryShaderPath,
		const std::filesystem::path& aPixelShaderPath) const;

	bool CreateShadowMap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture);
	bool CreateShadowCubemap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture);

	void SetRenderTarget(std::shared_ptr<Texture> aRenderTarget, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget = true, bool aClearDepthStencil = true);
	void SetRenderTargets(const std::vector<std::shared_ptr<Texture>>& aRenderTargets, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget = true, bool aClearDepthStencil = true);

	template <typename VertexType>
	bool CreateVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer) const;
	template <typename VertexType>
	bool CreateDynamicVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer, size_t aMaxVertexCount) const;
	bool CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer, bool aIsDynamic = false);

	template <typename VertexType>
	bool UpdateDynamicVertexBuffer(const std::vector<VertexType>& aVertexList, DynamicVertexBuffer& outVxBuffer) const;
	bool UpdateDynamicIndexBuffer(const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer);

	void BeginEvent(std::string_view aEvent) const { myRHI->BeginEvent(aEvent); }
	void EndEvent() const { myRHI->EndEvent(); }
	void SetMarker(std::string_view aMarker) const { myRHI->SetMarker(aMarker); }

	std::shared_ptr<Texture> GetBackBuffer() { return myRHI->GetBackBuffer(); }
	std::shared_ptr<Texture> GetDepthBuffer() { return myRHI->GetDepthBuffer(); }
	std::shared_ptr<Texture> GetIntermediateTexture(IntermediateTexture aIntermediateTexture);
	GBuffer& GetGBuffer() { return *myGBuffer; }

	GraphicsCommandList& GetGraphicsCommandList() const { return *myCommandList; }

	const unsigned GetDrawcallAmount() const { return myLastFrameDrawcallAmount; }

	Mesh CreatePlanePrimitive();
	Mesh CreateCubePrimitive();
	Mesh CreateRampPrimitive();

private:
	GraphicsEngine();
	~GraphicsEngine();

	void CreateConstantBuffers();

	std::unique_ptr<RenderHardwareInterface> myRHI;
	std::unique_ptr<PostProcessingSettings> myPostProcessingSettings;
	std::unique_ptr<Drawer> myDrawer;

	std::unordered_map<ConstantBufferType, ConstantBuffer> myConstantBuffers;
	
	std::shared_ptr<PipelineStateObject> myCurrentPSO;
	std::shared_ptr<PipelineStateObject> myDefaultPSO;

	std::shared_ptr<Texture> myLUTtexture;
	
	std::unique_ptr<GBuffer> myGBuffer;
	std::unique_ptr<GraphicsCommandList> myCommandList;

	unsigned myDrawcallAmount = 0;
	unsigned myLastFrameDrawcallAmount = 0;
};

template<typename BufferData>
bool GraphicsEngine::UpdateAndSetConstantBuffer(ConstantBufferType aBufferType, const BufferData& aDataBlock)
{
	if (!myConstantBuffers.contains(aBufferType))
	{
		return false;
	}

	ConstantBuffer& buffer = myConstantBuffers[aBufferType];
	if (!myRHI->UpdateConstantBuffer(buffer, aDataBlock))
	{
		return false;
	}

	myRHI->SetConstantBuffer(buffer);
	return true;
}

template<typename VertexType>
inline bool GraphicsEngine::CreateVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer) const
{
	return myRHI->CreateVertexBuffer(aName, aVertexList, outVxBuffer);
}

template<typename VertexType>
inline bool GraphicsEngine::CreateDynamicVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer, size_t aMaxVertexCount) const
{
	return myRHI->CreateDynamicVertexBuffer(aName, aVertexList, outVxBuffer, aMaxVertexCount);
}

template<typename VertexType>
inline bool GraphicsEngine::UpdateDynamicVertexBuffer(const std::vector<VertexType>& aVertexList, DynamicVertexBuffer& outVxBuffer) const
{
	return myRHI->UpdateDynamicVertexBuffer(aVertexList, outVxBuffer);
}
