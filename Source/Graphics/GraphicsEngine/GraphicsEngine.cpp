#include "GraphicsEngine.pch.h"
#include "GraphicsEngine.h"

#include "GameEngine/Engine.h"
#include "GraphicsEngine/Objects/Shader.h"
#include "GraphicsEngine/Objects/Vertices/Vertex.h"
#include "GraphicsEngine/Objects/Vertices/DebugLineVertex.h"
#include "GraphicsEngine/Objects/Mesh.h"
#include "GraphicsEngine/Objects/Text/Text.h"
#include "GraphicsEngine/Objects/Sprite.h"
#include "GraphicsEngine/Objects/Material.h"
#include "GraphicsEngine/Objects/Texture.h"
#include "GraphicsEngine/Objects/ConstantBuffers/FrameBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/ObjectBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/AnimationBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/MaterialBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/LightBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/ShadowBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/SpriteBuffer.h"

#include "GraphicsEngine/Objects/DynamicVertexBuffer.h"
#include "Objects/GBuffer.h"
#include "AssetManager/AssetManager.h"

GraphicsEngine& GraphicsEngine::Get()
{
	static GraphicsEngine myInstance;
	return myInstance;
}

bool GraphicsEngine::Initialize(HWND aWindowHandle)
{
	myRHI = std::make_unique<RenderHardwareInterface>();

	LOG(LogGraphicsEngine, Log, "Initializing Graphics Engine...");

	bool createDebugLayer = true;

#ifdef _RETAIL
	createDebugLayer = false;
#endif

	if (!myRHI->Initialize(aWindowHandle, createDebugLayer))
	{
		myRHI.reset();
		LOG(LogGraphicsEngine, Error, "Failed to initialize graphics engine!");
		return false;
	}

	{
		PipelineStateObject defaultPSO;
		defaultPSO.SamplerStates[0] = myRHI->GetSamplerState("LinearWrapSS");

		defaultPSO.VertexStride = sizeof(Vertex);

		std::wstring root = Engine::GetInstance().GetContentRootPath().wstring();
		if (!myRHI->CreateInputLayout(defaultPSO.InputLayout, Vertex::InputLayoutDefinition, root + L"EngineAssets/Shaders/SH_Mesh_VS.cso"))
		{
			LOG(LogGraphicsEngine, Error, "Failed to load default input layout!");
			return false;
		}

		defaultPSO.VertexShader = std::make_shared<Shader>();
		if (!myRHI->LoadShaderFromFilePath("Default_VS", *defaultPSO.VertexShader, root + L"EngineAssets/Shaders/SH_Mesh_VS.cso"))
		{
			LOG(LogGraphicsEngine, Error, "Failed to load default vertex shader!");
			return false;
		}

		defaultPSO.PixelShader = std::make_shared<Shader>();
		if (!myRHI->LoadShaderFromFilePath("Default_PS", *defaultPSO.PixelShader, root + L"EngineAssets/Shaders/SH_PBRMesh_PS.cso"))
		{
			LOG(LogGraphicsEngine, Error, "Failed to load default pixel shader!");
			return false;
		}

		myDefaultPSO = std::make_shared<PipelineStateObject>(defaultPSO);
		myCurrentPSO = myDefaultPSO;
	}

	myLUTtexture = std::make_shared<Texture>();
	myRHI->CreateLUT("LUT", 512, 512, myLUTtexture);

	CreateConstantBuffers();
	
	myCommandList = std::make_unique<GraphicsCommandList>();
	myGBuffer = std::make_unique<GBuffer>();
	CU::Vector2f resolution = Engine::GetInstance().GetResolution();
	myGBuffer->CreateGBuffer(static_cast<unsigned>(resolution.x), static_cast<unsigned>(resolution.y));

	LOG(LogGraphicsEngine, Log, "Initialized Graphics Engine!");
	return true;
}

#ifdef _DEBUG
bool GraphicsEngine::InitializeImGui()
{
	return myRHI->InitializeImGui();
}
#endif

void GraphicsEngine::BeginFrame()
{
	myLastFrameDrawcallAmount = myDrawcallAmount;
	myDrawcallAmount = 0;
	ChangePipelineState(myCurrentPSO);
}

void GraphicsEngine::RenderFrame()
{
	if (myCommandList->HasCommands() && !myCommandList->IsFinished())
	{
		myCommandList->Execute();
	}
	myCommandList->Reset();
}

void GraphicsEngine::EndFrame()
{
	myRHI->Present();
}

void GraphicsEngine::SetResolution(float aNewWidth, float aNewHeight)
{
	myRHI->SetResolution(aNewWidth, aNewHeight);
}

void GraphicsEngine::SetWindowSize(float aNewWidth, float aNewHeight)
{
	myRHI->SetWindowSize(aNewWidth, aNewHeight);
}

void GraphicsEngine::MaximizeWindowSize()
{
	myRHI->MaximizeWindowSize();
}

void GraphicsEngine::ChangePipelineState(const std::shared_ptr<PipelineStateObject> aNewPSO)
{
	myRHI->ChangePipelineState(*aNewPSO);
	myCurrentPSO = aNewPSO;
}

bool GraphicsEngine::CreateTexture(std::string_view aName, unsigned aWidth, unsigned aHeight, RHITextureFormat aFormat, Texture& outTexture, bool aStaging, bool aShaderResource, bool aRenderTarget, bool aCpuAccessRead, bool aCpuAccessWrite) const
{
	return myRHI->CreateTexture(aName, aWidth, aHeight, aFormat, outTexture, aStaging, aShaderResource, aRenderTarget, aCpuAccessRead, aCpuAccessWrite);
}

bool GraphicsEngine::LoadTexture(std::string_view aName, const uint8_t* aTextureDataPtr, size_t aTextureDataSize, Texture& outTexture) const
{
	return myRHI->LoadTexture(aName, aTextureDataPtr, aTextureDataSize, outTexture);
}

bool GraphicsEngine::LoadTexture(std::filesystem::path& aFilePath, Texture& outTexture) const
{
	std::ifstream file(aFilePath, std::ios::binary);
	file.seekg(0, std::ios::end);
	const std::streamoff fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<uint8_t> fileData;
	fileData.resize(fileSize);
	file.read(reinterpret_cast<char*>(fileData.data()), fileSize);
	file.close();
	
	return myRHI->LoadTexture(aFilePath.filename().string(), fileData.data(), static_cast<size_t>(fileSize), outTexture);
}

bool GraphicsEngine::SetTextureResource_PS(unsigned aSlot, Texture& aTexture)
{
	return myRHI->SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, aSlot, aTexture);
}

bool GraphicsEngine::SetTextureResource_VS(unsigned aSlot, Texture& aTexture)
{
	return myRHI->SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER, aSlot, aTexture);
}

bool GraphicsEngine::ClearTextureResource_PS(unsigned aSlot)
{
	return myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, aSlot);
}

bool GraphicsEngine::ClearTextureResource_VS(unsigned aSlot)
{
	return myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_VERTEX_SHADER, aSlot);
}

bool GraphicsEngine::LoadShader(std::filesystem::path aFilePath, Shader& outShader)
{
	return myRHI->LoadShaderFromFilePath(aFilePath.stem().string(), outShader, aFilePath.generic_wstring());
}

bool GraphicsEngine::CreatePSO(std::shared_ptr<PipelineStateObject> aPSO, PSODescription& aPSOdesc)
{
	aPSO->VertexStride = aPSOdesc.vertexStride;

	if (aPSOdesc.vsPath != L"")
	{
		if (!myRHI->CreateInputLayout(aPSO->InputLayout, aPSOdesc.inputLayoutDefinition, aPSOdesc.vsPath))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create PSO!");
			return false;
		}
	}

	aPSO->VertexShader = aPSOdesc.vsShader;
	aPSO->GeometryShader = aPSOdesc.gsShader;
	aPSO->PixelShader = aPSOdesc.psShader;

	if (!(aPSOdesc.fillMode == 3 && aPSOdesc.cullMode == 3 && aPSOdesc.antiAliasedLine == false))
	{
		D3D11_RASTERIZER_DESC rastDesc = {};
		rastDesc.FillMode = static_cast<D3D11_FILL_MODE>(aPSOdesc.fillMode);
		rastDesc.CullMode = static_cast<D3D11_CULL_MODE>(aPSOdesc.cullMode);
		rastDesc.AntialiasedLineEnable = aPSOdesc.antiAliasedLine;

		if (!myRHI->CreateRasterizerState(aPSOdesc.name + "_Rasterizer", rastDesc, *aPSO))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create rasterizer for PSO {}!", aPSOdesc.name);
			return false;
		}
	}


	if (aPSOdesc.blendMode != BlendMode::None)
	{
		CD3D11_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = aPSOdesc.alphaToCoverage;
		blendDesc.IndependentBlendEnable = aPSOdesc.independentBlend;

		D3D11_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc = {};
		defaultRenderTargetBlendDesc.BlendEnable = TRUE;
		defaultRenderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		if (aPSOdesc.blendMode == BlendMode::AlphaAdditive)
		{
			defaultRenderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			defaultRenderTargetBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			defaultRenderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;

			defaultRenderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
			defaultRenderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;
			defaultRenderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		}
		else if (aPSOdesc.blendMode == BlendMode::Additive)
		{
			defaultRenderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			defaultRenderTargetBlendDesc.DestBlend = D3D11_BLEND_ONE;
			defaultRenderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;

			defaultRenderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
			defaultRenderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;
			defaultRenderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_MAX;
		}

		for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			blendDesc.RenderTarget[i] = defaultRenderTargetBlendDesc;
		}

		if (!myRHI->CreateBlendState(aPSOdesc.name + "_BlendState", blendDesc, *aPSO))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create blend state for PSO {}!", aPSOdesc.name);
			return false;
		}

		LOG(LogGraphicsEngine, Log, "Successfully created blend state for PSO {}!", aPSOdesc.name);
	}

	for (auto& sampler : aPSOdesc.samplerList)
	{
		aPSO->SamplerStates[sampler.first] = myRHI->GetSamplerState(sampler.second);
	}

	LOG(LogGraphicsEngine, Log, "Created PSO {}!", aPSOdesc.name);
	return true;
}

bool GraphicsEngine::CreateShadowMap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture)
{
	return myRHI->CreateShadowMap(aName, aWidth, aHeight, outTexture);
}

bool GraphicsEngine::CreateShadowCubemap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture)
{
	return myRHI->CreateShadowCubemap(aName, aWidth, aHeight, outTexture);
}

void GraphicsEngine::SetRenderTarget(std::shared_ptr<Texture> aRenderTarget, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget, bool aClearDepthStencil)
{
	myRHI->SetRenderTarget(aRenderTarget, aDepthStencil, aClearRenderTarget, aClearDepthStencil);
}

void GraphicsEngine::SetRenderTargets(std::vector<std::shared_ptr<Texture>> aRenderTargets, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget, bool aClearDepthStencil)
{
	myRHI->SetRenderTargets(aRenderTargets, aDepthStencil, aClearRenderTarget, aClearDepthStencil);
}

void GraphicsEngine::RenderQuad()
{
	SetTextureResource_PS(127, *myLUTtexture);

	myRHI->SetPrimitiveTopology(Topology::TRIANGLESTRIP);
	myRHI->SetVertexBuffer(nullptr, 0, 0);
	myRHI->SetIndexBuffer(nullptr);
	myRHI->SetInputLayout(nullptr);
	myRHI->Draw(4);
	ClearTextureResource_PS(127);
}

void GraphicsEngine::RenderMesh(const Mesh& aMesh, std::vector<std::shared_ptr<Material>> aMaterialList, bool aOverrideMaterialPSO)
{
	myRHI->SetVertexBuffer(aMesh.GetVertexBuffer(), myCurrentPSO->VertexStride, 0);
	myRHI->SetIndexBuffer(aMesh.GetIndexBuffer());
	myRHI->SetPrimitiveTopology(Topology::TRIANGLELIST);

	SetTextureResource_PS(127, *myLUTtexture);

	for (const auto& element : aMesh.GetElements())
	{
		if (aMaterialList.size() > element.MaterialIndex)
		{
			MaterialBuffer matBufferData = aMaterialList[element.MaterialIndex]->MaterialSettings();
			GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer, matBufferData);

			if (!aOverrideMaterialPSO)
			{
				if (aMaterialList[element.MaterialIndex]->GetPSO())
				{
					ChangePipelineState(aMaterialList[element.MaterialIndex]->GetPSO());
				}
			}

			SetTextureResource_PS(0, aMaterialList[element.MaterialIndex]->GetAlbedoTexture());
			SetTextureResource_PS(1, aMaterialList[element.MaterialIndex]->GetNormalTexture());
			SetTextureResource_PS(2, aMaterialList[element.MaterialIndex]->GetMaterialTexture());
		}

		myRHI->DrawIndexed(element.IndexOffset, element.NumIndices);
		myDrawcallAmount++;
	}

	ClearTextureResource_PS(0);
	ClearTextureResource_PS(1);
	ClearTextureResource_PS(2);
	ClearTextureResource_PS(127);
}

void GraphicsEngine::RenderSprite()
{
	myRHI->SetPrimitiveTopology(Topology::POINTLIST);
	myRHI->Draw(1);
}

void GraphicsEngine::RenderText(const Text& aText)
{
	SetTextureResource_PS(3, *aText.GetTexture());

	const Text::TextData& textData = aText.GetTextData();
	myRHI->SetVertexBuffer(textData.vertexBuffer->GetVertexBuffer(), myCurrentPSO->VertexStride, 0);
	myRHI->SetIndexBuffer(textData.indexBuffer);
	myRHI->SetPrimitiveTopology(Topology::TRIANGLELIST);

	myRHI->DrawIndexed(0, textData.numIndices);
	myDrawcallAmount++;

	ClearTextureResource_PS(3);
}

void GraphicsEngine::RenderDebugLines(DynamicVertexBuffer& aDynamicBuffer, unsigned aLineAmount)
{
	myRHI->SetVertexBuffer(aDynamicBuffer.GetVertexBuffer(), myCurrentPSO->VertexStride, 0);
	myRHI->SetPrimitiveTopology(Topology::POINTLIST);
	myRHI->Draw(aLineAmount);
}

bool GraphicsEngine::CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer, bool aIsDynamic)
{
	return myRHI->CreateIndexBuffer(aName, aIndexList, outIxBuffer, aIsDynamic);
}

bool GraphicsEngine::UpdateDynamicIndexBuffer(const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer)
{
	return myRHI->UpdateDynamicIndexBuffer(aIndexList, outIxBuffer);
}

GraphicsEngine::GraphicsEngine() = default;
GraphicsEngine::~GraphicsEngine() = default;

void GraphicsEngine::CreateConstantBuffers()
{
	ConstantBuffer frameBuffer;
	myRHI->CreateConstantBuffer("FrameBuffer", sizeof(FrameBuffer), 0, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_GEOMETRY_SHADER | PIPELINE_STAGE_PIXEL_SHADER, frameBuffer);
	myConstantBuffers.emplace(ConstantBufferType::FrameBuffer, std::move(frameBuffer));

	ConstantBuffer objectBuffer;
	myRHI->CreateConstantBuffer("ObjectBuffer", sizeof(ObjectBuffer), 1, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_GEOMETRY_SHADER | PIPELINE_STAGE_PIXEL_SHADER, objectBuffer);
	myConstantBuffers.emplace(ConstantBufferType::ObjectBuffer, std::move(objectBuffer));

	ConstantBuffer animationBuffer;
	myRHI->CreateConstantBuffer("AnimationBuffer", sizeof(AnimationBuffer), 2, PIPELINE_STAGE_VERTEX_SHADER, animationBuffer);
	myConstantBuffers.emplace(ConstantBufferType::AnimationBuffer, std::move(animationBuffer));

	ConstantBuffer materialBuffer;
	myRHI->CreateConstantBuffer("MaterialBuffer", sizeof(MaterialBuffer), 3, PIPELINE_STAGE_PIXEL_SHADER, materialBuffer);
	myConstantBuffers.emplace(ConstantBufferType::MaterialBuffer, std::move(materialBuffer));

	ConstantBuffer lightBuffer;
	myRHI->CreateConstantBuffer("LightBuffer", sizeof(LightBuffer), 4, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER, lightBuffer);
	myConstantBuffers.emplace(ConstantBufferType::LightBuffer, std::move(lightBuffer));

	ConstantBuffer shadowBuffer;
	myRHI->CreateConstantBuffer("ShadowBuffer", sizeof(ShadowBuffer), 5, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_GEOMETRY_SHADER | PIPELINE_STAGE_PIXEL_SHADER, shadowBuffer);
	myConstantBuffers.emplace(ConstantBufferType::ShadowBuffer, std::move(shadowBuffer));

	ConstantBuffer spriteBuffer;
	myRHI->CreateConstantBuffer("SpriteBuffer", sizeof(SpriteBuffer), 6, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_GEOMETRY_SHADER | PIPELINE_STAGE_PIXEL_SHADER, spriteBuffer);
	myConstantBuffers.emplace(ConstantBufferType::SpriteBuffer, std::move(spriteBuffer));
}
