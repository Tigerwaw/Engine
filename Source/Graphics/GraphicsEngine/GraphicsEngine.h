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

class Shader;
class Mesh;
class Text;
class Sprite;
class DynamicVertexBuffer;
class GraphicsCommandList;
struct GraphicsCommandBase;
class GBuffer;

struct ID3D11InputLayout;

struct Vertex;

enum class DebugMode
{
	None,
	Unlit,
	DebugAO,
	DebugRoughness,
	DebugMetallic,
	DebugFX,
	Wireframe,
	DebugVertexNormals,
	DebugVertexTangents,
	DebugVertexBinormals,
	DebugPixelNormals,
	DebugTextureNormals,
	DebugUVs,
	DebugVertexColor,
	COUNT
};

enum class Tonemapper
{
	UE,
	ACES,
	Lottes,
	COUNT
};

enum class Luminance
{
	RandomGain,
	Contrast,
	ReductionAndGain,
	Fade
};

enum class Bloom
{
	Additive,
	ScaledToScene,
	ScaledToLuminance
};

enum class ConstantBufferType
{
	ObjectBuffer,
	FrameBuffer,
	AnimationBuffer,
	MaterialBuffer,
	LightBuffer,
	ShadowBuffer,
	SpriteBuffer
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
};

class GraphicsEngine
{
public:
	static GraphicsEngine& Get();

	bool Initialize(HWND aWindowHandle);
#ifndef _RETAIL
	bool InitializeImGui();
#endif
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
	bool LoadTexture(std::filesystem::path& aFilePath, Texture& outTexture) const;
	bool SetTextureResource_PS(unsigned aSlot, Texture& aTexture);
	bool SetTextureResource_VS(unsigned aSlot, Texture& aTexture);
	bool ClearTextureResource_PS(unsigned aSlot);
	bool ClearTextureResource_VS(unsigned aSlot);

	bool LoadShader(std::filesystem::path aFilePath, Shader& outShader);
	bool CreatePSO(std::shared_ptr<PipelineStateObject> aPSO, PSODescription& aPSOdesc);
	std::shared_ptr<PipelineStateObject> GetDefaultPSO() { return myDefaultPSO; }

	bool CreateShadowMap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture);
	bool CreateShadowCubemap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture);

	void SetRenderTarget(std::shared_ptr<Texture> aRenderTarget, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget = true, bool aClearDepthStencil = true);
	void SetRenderTargets(std::vector<std::shared_ptr<Texture>> aRenderTargets, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget = true, bool aClearDepthStencil = true);

	void RenderQuad();
	void RenderMesh(const Mesh& aMesh, std::vector<std::shared_ptr<Material>> aMaterialList, bool aOverrideMaterialPSO = false);
	void RenderSprite();
	void RenderText(const Text& aText);
	void RenderDebugLines(DynamicVertexBuffer& aDynamicBuffer, unsigned aLineAmount);

	template <typename VertexType>
	bool CreateVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer) const;
	template <typename VertexType>
	bool CreateDynamicVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer, size_t aMaxVertexCount) const;
	bool CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer, bool aIsDynamic = false);

	template <typename VertexType>
	bool UpdateDynamicVertexBuffer(const std::vector<VertexType>& aVertexList, DynamicVertexBuffer& outVxBuffer) const;
	bool UpdateDynamicIndexBuffer(const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer);

	std::shared_ptr<Texture> GetBackBuffer() { return myRHI->GetBackBuffer(); }
	std::shared_ptr<Texture> GetDepthBuffer() { return myRHI->GetDepthBuffer(); }
	std::shared_ptr<Texture> GetIntermediateHDRBuffer() { return myRHI->GetIntermediateHDRBuffer(); }
	std::shared_ptr<Texture> GetIntermediateLDRBuffer() { return myRHI->GetIntermediateLDRBuffer(); }
	std::shared_ptr<Texture> GetLuminanceBuffer() { return myRHI->GetLuminanceBuffer(); }
	std::shared_ptr<Texture> GetHalfScreenBuffer() { return myRHI->GetHalfScreenBuffer(); }
	std::shared_ptr<Texture> GetQuarterScreenBufferA() { return myRHI->GetQuarterScreenBufferA(); }
	std::shared_ptr<Texture> GetQuarterScreenBufferB() { return myRHI->GetQuarterScreenBufferB(); }
	GBuffer& GetGBuffer() { return *myGBuffer; }

	GraphicsCommandList& GetGraphicsCommandList() const { return *myCommandList; }

	const unsigned GetDrawcallAmount() const { return myLastFrameDrawcallAmount; }
	void SetDebugMode(DebugMode aDebugMode) { myCurrentDebugMode = aDebugMode; }
	const DebugMode GetCurrentDebugMode() const { return myCurrentDebugMode; }

	void SetTonemapper(Tonemapper aTonemapper) { myTonemapper = aTonemapper; }
	const Tonemapper GetTonemapper() const { return myTonemapper; }

	std::vector<std::string> DebugModeNames = {
		"None",
		"PSO_Unlit",
		"PSO_DebugAO",
		"PSO_DebugRoughness",
		"PSO_DebugMetallic",
		"PSO_DebugFX",
		"PSO_Wireframe",
		"PSO_DebugVertexNormals",
		"PSO_DebugVertexTangents",
		"PSO_DebugVertexBinormals",
		"PSO_DebugPixelNormals",
		"PSO_DebugTextureNormals",
		"PSO_DebugUVs",
		"PSO_DebugVertexColor"
	};
	
	std::vector<std::string> TonemapperNames = {
		"PSO_TonemapUE",
		"PSO_TonemapACES",
		"PSO_TonemapLottes"
	};

	bool DrawGizmos = false;
	bool DrawBoundingBoxes = false;
	bool DrawCameraFrustums = false;
	bool DrawColliders = false;
	bool UseViewCulling = true;
	bool RecalculateShadowFrustum = true;

private:
	GraphicsEngine();
	~GraphicsEngine();

	void CreateConstantBuffers();

	std::unique_ptr<RenderHardwareInterface> myRHI;
	std::unordered_map<ConstantBufferType, ConstantBuffer> myConstantBuffers;
	
	std::shared_ptr<PipelineStateObject> myCurrentPSO;
	std::shared_ptr<PipelineStateObject> myDefaultPSO;

	std::shared_ptr<Texture> myLUTtexture;
	
	std::unique_ptr<GBuffer> myGBuffer;
	std::unique_ptr<GraphicsCommandList> myCommandList;

	unsigned myDrawcallAmount = 0;
	unsigned myLastFrameDrawcallAmount = 0;
	Tonemapper myTonemapper = Tonemapper::UE;
	DebugMode myCurrentDebugMode = DebugMode::None;
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
