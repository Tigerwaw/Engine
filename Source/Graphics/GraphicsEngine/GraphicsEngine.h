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
#include "Graphics/GraphicsEngine/PipelineStateType.h"

class Shader;
class Mesh;
class Sprite;
class GraphicsCommandList;
struct GraphicsCommandBase;

struct ID3D11InputLayout;

struct Vertex;

enum class DebugMode
{
	None,
	Unlit,
	Wireframe,
	DebugVertexNormals,
	DebugPixelNormals,
	DebugTextureNormals,
	DebugUVs
};

enum class ConstantBufferType
{
	ObjectBuffer,
	FrameBuffer,
	AnimationBuffer,
	MaterialBuffer,
	LightBuffer,
	ShadowBuffer,
	SpriteBuffer,
	DebugBuffer
};

class GraphicsEngine
{
public:
	static GraphicsEngine& Get();

	bool Initialize(HWND aWindowHandle);
	void BeginFrame();
	void RenderFrame();
	void EndFrame();

	template<typename BufferData>
	bool UpdateAndSetConstantBuffer(ConstantBufferType aBufferType, const BufferData& aDataBlock);

	void ChangePipelineState(PipelineStateType aPipelineState);
	void ChangePipelineState(const std::shared_ptr<PipelineStateObject> aNewPSO);
	std::shared_ptr<PipelineStateObject> GetPSO(PipelineStateType aPipelineState);

	bool LoadTexture(std::string_view aName, const uint8_t* aTextureDataPtr, size_t aTextureDataSize, Texture& outTexture) const;
	bool LoadTexture(std::filesystem::path& aFilePath, Texture& outTexture) const;
	bool SetTextureResource_PS(unsigned aSlot, Texture& aTexture);
	bool SetTextureResource_VS(unsigned aSlot, Texture& aTexture);
	bool ClearTextureResource_PS(unsigned aSlot);
	bool ClearTextureResource_VS(unsigned aSlot);

	bool CreateShadowMap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture);
	bool CreateShadowCubemap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture);

	void SetRenderTarget(std::shared_ptr<Texture> aRenderTarget, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget = true, bool aClearDepthStencil = true);

	void RenderMesh(const Mesh& aMesh, std::vector<std::shared_ptr<Material>> aMaterialList);
	void RenderSprite();
	void RenderDebugLine(unsigned aLineAmount);

	template <typename VertexType>
	bool CreateVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer) const;
	bool CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer);

	std::shared_ptr<Texture> GetBackBuffer() { return myRHI->GetBackBuffer(); }
	std::shared_ptr<Texture> GetDepthBuffer() { return myRHI->GetDepthBuffer(); }

	GraphicsCommandList& GetGraphicsCommandList() const { return *myCommandList; }

	const unsigned GetDrawcallAmount() const { return myDrawcallAmount; }
	const DebugMode GetCurrentDebugMode() const { return myCurrentDebugMode; }
	void SetDebugMode(DebugMode aDebugMode) { myCurrentDebugMode = aDebugMode; }

	std::vector<std::string> DebugModeNames = {
		"None",
		"Unlit",
		"Wireframe",
		"DebugVertexNormals",
		"DebugPixelNormals",
		"DebugTextureNormals",
		"DebugUVs"
	};
	bool DrawGizmos = false;
	bool DrawBoundingBoxes = false;
	bool DrawCameraFrustums = false;
	bool UseViewCulling = true;

private:
	GraphicsEngine();
	~GraphicsEngine();

	void CreateConstantBuffers();

	std::unique_ptr<RenderHardwareInterface> myRHI;
	std::unordered_map<ConstantBufferType, ConstantBuffer> myConstantBuffers;
	
	std::shared_ptr<PipelineStateObject> myCurrentPSO;
	std::unordered_map<PipelineStateType, std::shared_ptr<PipelineStateObject>> myPSOmap;

	std::shared_ptr<Texture> myLUTtexture;
	
	std::unique_ptr<GraphicsCommandList> myCommandList;

	unsigned myDrawcallAmount = 0;
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
