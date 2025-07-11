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
#include "ResourceVendor.h"

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
	ResourceVendor& GetResourceVendor() { return *myResourceVendor; }

	void BeginFrame();
	void RenderFrame();
	void EndFrame();

	void SetResolution(float aNewWidth, float aNewHeight);
	void SetWindowSize(float aNewWidth, float aNewHeight);
	void MaximizeWindowSize();

	template<typename BufferData>
	bool UpdateAndSetConstantBuffer(ConstantBufferType aBufferType, const BufferData& aDataBlock);

	void ChangePipelineState(const std::shared_ptr<PipelineStateObject> aNewPSO);

	bool SetTextureResource_PS(unsigned aSlot, Texture& aTexture);
	bool SetTextureResource_VS(unsigned aSlot, Texture& aTexture);
	bool ClearTextureResource_PS(unsigned aSlot);
	bool ClearTextureResource_VS(unsigned aSlot);

	void SetRenderTarget(std::shared_ptr<Texture> aRenderTarget, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget = true, bool aClearDepthStencil = true);
	void SetRenderTargets(const std::vector<std::shared_ptr<Texture>>& aRenderTargets, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget = true, bool aClearDepthStencil = true);

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
	std::shared_ptr<PipelineStateObject> GetDefaultPSO() { return myDefaultPSO; }

	const unsigned GetDrawcallAmount() const { return myLastFrameDrawcallAmount; }

private:
	GraphicsEngine();
	~GraphicsEngine();

	void CreateConstantBuffers();

	std::shared_ptr<RenderHardwareInterface> myRHI;
	std::unique_ptr<PostProcessingSettings> myPostProcessingSettings;
	std::unique_ptr<Drawer> myDrawer;
	std::unique_ptr<ResourceVendor> myResourceVendor;

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
inline bool GraphicsEngine::UpdateDynamicVertexBuffer(const std::vector<VertexType>& aVertexList, DynamicVertexBuffer& outVxBuffer) const
{
	return myRHI->UpdateDynamicVertexBuffer(aVertexList, outVxBuffer);
}
