#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <wrl.h>
#include "RHI/RenderHardwareInterface.h"
#include "RHI/PipelineStateObject.h"
#include "RHI/GraphicsCommands/GraphicsCommandList.h"
#include "Objects/ConstantBuffers/ConstantBuffer.h"
#include "Graphics/GraphicsEngine/PipelineStateType.h"

class Shader;
class Mesh;
class GraphicsCommandList;
struct GraphicsCommandBase;

struct ID3D11InputLayout;

struct Vertex;

enum class ConstantBufferType
{
	ObjectBuffer,
	FrameBuffer,
	AnimationBuffer,
	MaterialBuffer,
	LightBuffer,
	ShadowBuffer
};

class GraphicsEngine
{
public:
	static GraphicsEngine& Get();

	bool Initialize(HWND aWindowHandle);
	void BeginFrame();
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

	void RenderMesh(const Mesh& aMesh, std::vector<std::shared_ptr<Material>> aMaterialList);

	template <typename VertexType>
	bool CreateVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer) const;
	bool CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer);

	const unsigned GetDrawcallAmount() const { return myDrawcallAmount; }

private:
	GraphicsEngine();
	~GraphicsEngine();

	std::unique_ptr<RenderHardwareInterface> myRHI;
	std::unordered_map<ConstantBufferType, ConstantBuffer> myConstantBuffers;
	
	std::shared_ptr<PipelineStateObject> myCurrentPSO;
	std::unordered_map<PipelineStateType, std::shared_ptr<PipelineStateObject>> myPSOmap;

	std::shared_ptr<Texture> myLUTtexture;

	std::shared_ptr<GraphicsCommandList> myCommandList;

	unsigned myDrawcallAmount = 0;
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
