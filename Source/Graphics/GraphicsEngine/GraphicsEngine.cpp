#include "GraphicsEngine.pch.h"
#include "GraphicsEngine.h"

#include "ShaderReflection/ShaderInfo.h"

#include "Objects/Shader.h"
#include "Objects/Vertices/Vertex.h"
#include "Objects/Vertices/DebugLineVertex.h"
#include "Objects/Mesh.h"
#include "Objects/Text/Text.h"
#include "Objects/Sprite.h"
#include "Objects/Material.h"
#include "Objects/Texture.h"
#include "Objects/ParticleSystem/ParticleEmitter.h"
#include "Objects/ParticleSystem/TrailEmitter.h"
#include "Objects/ConstantBuffers/FrameBuffer.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/AnimationBuffer.h"
#include "Objects/ConstantBuffers/MaterialBuffer.h"
#include "Objects/ConstantBuffers/LightBuffer.h"
#include "Objects/ConstantBuffers/ShadowBuffer.h"
#include "Objects/ConstantBuffers/SpriteBuffer.h"
#include "Objects/ConstantBuffers/PostProcessBuffer.h"

#include "Objects/DynamicVertexBuffer.h"
#include "Objects/GBuffer.h"

bool GraphicsEngine::Initialize(HWND aWindowHandle, Math::Vector2f aResolution, const std::filesystem::path& aContentRoot)
{
	myRHI = std::make_unique<RenderHardwareInterface>();
	myPostProcessingSettings = std::make_unique<PostProcessingSettings>();
	myDrawer = std::make_unique<Drawer>();

	LOG(LogGraphicsEngine, Log, "Initializing Graphics Engine...");

	bool createDebugLayer = false;

#ifdef _DEBUG
	createDebugLayer = true;
#endif

	RenderHardwareInterface& rhi = *myRHI;
	if (!rhi.Initialize(aWindowHandle, createDebugLayer))
	{
		myRHI = nullptr;
		LOG(LogGraphicsEngine, Error, "Failed to initialize RHI!");
		return false;
	}

	{
		PipelineStateObject defaultPSO;
		defaultPSO.SamplerStates[0] = rhi.GetSamplerState("LinearWrapSS");

		defaultPSO.VertexStride = sizeof(Vertex);

		std::wstring root = aContentRoot.wstring();
		if (!rhi.CreateInputLayout(defaultPSO.InputLayout, Vertex::InputLayoutDefinition, root + L"EngineAssets/Shaders/Mesh_VS.cso"))
		{
			LOG(LogGraphicsEngine, Error, "Failed to load default input layout!");
			return false;
		}

		defaultPSO.VertexShader = std::make_shared<Shader>();
		if (!rhi.LoadShaderFromFilePath("Default_VS", *defaultPSO.VertexShader, root + L"EngineAssets/Shaders/Mesh_VS.cso"))
		{
			LOG(LogGraphicsEngine, Error, "Failed to load default vertex shader!");
			return false;
		}

		defaultPSO.PixelShader = std::make_shared<Shader>();
		if (!rhi.LoadShaderFromFilePath("Default_PS", *defaultPSO.PixelShader, root + L"EngineAssets/Shaders/Deferred_PS.cso"))
		{
			LOG(LogGraphicsEngine, Error, "Failed to load default pixel shader!");
			return false;
		}

		myDefaultPSO = std::make_shared<PipelineStateObject>(defaultPSO);
		myCurrentPSO = myDefaultPSO;
	}

	myLUTtexture = std::make_shared<Texture>();
	rhi.CreateLUT("LUT", 512, 512, myLUTtexture, aContentRoot / L"EngineAssets/Shaders/Quad_VS.cso", aContentRoot / L"EngineAssets/Shaders/brdfLUT_PS.cso");

	CreateConstantBuffers();

	myPostProcessingSettings->CreateRandomKernel(64);
	
	myCommandList = std::make_unique<GraphicsCommandList>();
	myGBuffer = std::make_unique<GBuffer>();
	Math::Vector2f resolution = aResolution;
	myGBuffer->CreateGBuffer(static_cast<unsigned>(resolution.x), static_cast<unsigned>(resolution.y));

	LOG(LogGraphicsEngine, Log, "Initialized Graphics Engine!");
	return true;
}

#ifndef _RETAIL
bool GraphicsEngine::InitializeImGui()
{
	return myRHI->InitializeImGui();
}
#endif

void GraphicsEngine::BeginFrame()
{
	myLastFrameDrawcallAmount = myDrawcallAmount;
	myDrawcallAmount = 0;
}

void GraphicsEngine::RenderFrame()
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "Execute Render Command List");
	if (myCommandList->HasCommands() && !myCommandList->IsFinished())
	{
		myCommandList->Execute();
	}
	PIXScopedEvent(PIX_COLOR_INDEX(1), "Reset Render Command List");
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

bool GraphicsEngine::LoadTexture(const std::filesystem::path& aFilePath, Texture& outTexture) const
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

bool GraphicsEngine::LoadShader(const std::filesystem::path& aFilePath, Shader& outShader)
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

		if (aPSOdesc.blendMode == BlendMode::Alpha)
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

	if (aPSOdesc.useReadOnlyDepthStencilState)
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = CD3D11_DEPTH_STENCIL_DESC();
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = false;

		if (!myRHI->CreateDepthStencilState(aPSOdesc.name + "_DepthStencilState", depthStencilDesc, *aPSO))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create depth stencil state for PSO {}!", aPSOdesc.name);
			return false;
		}
	}

	for (auto& sampler : aPSOdesc.samplerList)
	{
		aPSO->SamplerStates[sampler.first] = myRHI->GetSamplerState(sampler.second);
	}

	LOG(LogGraphicsEngine, Log, "Created PSO {}!", aPSOdesc.name);
	return true;
}

const bool GraphicsEngine::CompareShaderParameters(const std::filesystem::path& aShaderOnePath, const std::filesystem::path& aShaderTwoPath) const
{
	ShaderInfo shaderOneInfo = myRHI->GetShaderInfo(aShaderOnePath);
	ShaderInfo shaderTwoInfo = myRHI->GetShaderInfo(aShaderTwoPath);

	if (shaderOneInfo.GetOutputParameters().size() != shaderTwoInfo.GetInputParameters().size())
	{
		LOG(LogGraphicsEngine, Error, "Output of shader {} ({} outputs) and input of shader {} ({} inputs) do not match!", aShaderOnePath.stem().string(), shaderOneInfo.GetOutputParameters().size(), aShaderTwoPath.stem().string(), shaderTwoInfo.GetInputParameters().size());
		return false;
	}

	for (size_t i = 0; i < shaderOneInfo.GetOutputParameters().size(); i++)
	{
		auto& outParam = shaderOneInfo.GetOutputParameters()[i];
		auto& inParam = shaderTwoInfo.GetInputParameters()[i];
		if (outParam != inParam)
		{
			LOG(LogGraphicsEngine, Error, "Output of shader {} and input of shader {} do not match! {} != {}", aShaderOnePath.stem().string(), aShaderTwoPath.stem().string(), outParam.SemanticName, inParam.SemanticName);
			return false;
		}
	}

	return true;
}

const bool GraphicsEngine::ValidateShaderCombination(const std::filesystem::path& aVertexShaderPath, const std::filesystem::path& aGeometryShaderPath, const std::filesystem::path& aPixelShaderPath) const
{
	bool hasVertexShader = aVertexShaderPath != "";
	bool hasGeometryShader = aGeometryShaderPath != "";
	bool hasPixelShader = aPixelShaderPath != "";

	if (hasVertexShader && hasGeometryShader && hasPixelShader)
	{
		return (CompareShaderParameters(aVertexShaderPath, aGeometryShaderPath) && CompareShaderParameters(aGeometryShaderPath, aPixelShaderPath));
	}
	else if (hasVertexShader && hasPixelShader)
	{
		return CompareShaderParameters(aVertexShaderPath, aPixelShaderPath);
	}
	else if (hasVertexShader && hasGeometryShader)
	{
		return CompareShaderParameters(aVertexShaderPath, aGeometryShaderPath);
	}

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

void GraphicsEngine::SetRenderTargets(const std::vector<std::shared_ptr<Texture>>& aRenderTargets, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget, bool aClearDepthStencil)
{
	myRHI->SetRenderTargets(aRenderTargets, aDepthStencil, aClearRenderTarget, aClearDepthStencil);
}

bool GraphicsEngine::CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer, bool aIsDynamic)
{
	return myRHI->CreateIndexBuffer(aName, aIndexList, outIxBuffer, aIsDynamic);
}

bool GraphicsEngine::UpdateDynamicIndexBuffer(const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer)
{
	return myRHI->UpdateDynamicIndexBuffer(aIndexList, outIxBuffer);
}

std::shared_ptr<Texture> GraphicsEngine::GetIntermediateTexture(IntermediateTexture aIntermediateTexture)
{
	return myRHI->GetIntermediateTexture(aIntermediateTexture);
}

Mesh GraphicsEngine::CreatePlanePrimitive()
{
	float emptyColor[4][4] = { {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
	float uv[4][4][2] = { {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}}
	};
	float pos[4][4] = {
		{1, 0, 1, 1},
		{1, 0, -1, 1},
		{-1, 0, -1, 1},
		{-1, 0, 1, 1}
	};
	float normals[4][3] = {
		{ 0, 1, 0 },
		{ 0, 1, 0 },
		{ 0, 1, 0 },
		{ 0, 1, 0 }
	};
	float tangents[4][3] = {
		{ 1, 0, 0 },
		{ 1, 0, 0 },
		{ 1, 0, 0 },
		{ 1, 0, 0 }
	};

	std::vector<Vertex> vertexList;
	vertexList.reserve(4);
	vertexList.emplace_back(pos[0], emptyColor, uv[0], normals[0], tangents[0]);
	vertexList.emplace_back(pos[1], emptyColor, uv[1], normals[1], tangents[1]);
	vertexList.emplace_back(pos[2], emptyColor, uv[2], normals[2], tangents[2]);
	vertexList.emplace_back(pos[3], emptyColor, uv[3], normals[3], tangents[3]);

	std::vector<unsigned> indexList = {
		0, 1, 2,
		2, 3, 0,
	};

	std::vector<Mesh::Element> elementList;
	Mesh::Element& element = elementList.emplace_back();
	element.VertexOffset = 0;
	element.IndexOffset = 0;
	element.NumVertices = static_cast<unsigned>(vertexList.size());
	element.NumIndices = static_cast<unsigned>(indexList.size());

	Mesh plane;
	plane.InitBoundingBox({ -1.0f, -0.001f, -1.0f }, { 1.0f, 0.001f, 1.0f });
	plane.Initialize(std::move(vertexList), std::move(indexList), std::move(elementList), Mesh::Skeleton());
	return plane;
}

Mesh GraphicsEngine::CreateCubePrimitive()
{
	float emptyColor[4][4] = { {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
	float uv[24][4][2] = { {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

						  {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

						  {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

						  {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

						  {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

						  {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						  {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
	};

	float pos[24][4] = {
		{1,1,-1, 1}, // BACK
		{1,-1,-1, 1},
		{-1,-1,-1, 1},
		{-1,1,-1, 1},

		{1,1,1, 1}, // FRONT
		{-1,1,1, 1},
		{-1,-1,1, 1},
		{1,-1,1, 1},

		{1,1,-1, 1}, // RIGHT
		{1,1,1, 1},
		{1,-1,1, 1},
		{1,-1,-1, 1},

		{1,-1,-1, 1}, // DOWN
		{1,-1,1, 1},
		{-1,-1,1, 1},
		{-1,-1,-1, 1},

		{-1,-1,-1, 1}, // LEFT
		{-1,-1,1, 1},
		{-1,1,1, 1},
		{-1,1,-1, 1},

		{1,1,1, 1}, // UP
		{1,1,-1, 1},
		{-1,1,-1, 1},
		{-1,1,1, 1},
	};

	float normals[24][3] = {
	{ 0, 0, -1 }, // BACK
	{ 0, 0, -1 },
	{ 0, 0, -1 },
	{ 0, 0, -1 },

	{ 0, 0, 1 }, // FRONT
	{ 0, 0, 1 },
	{ 0, 0, 1 },
	{ 0, 0, 1 },

	{ 1, 0, 0 }, // RIGHT
	{ 1, 0, 0 },
	{ 1, 0, 0 },
	{ 1, 0, 0 },

	{ 0, -1, 0 }, // DOWN
	{ 0, -1, 0 },
	{ 0, -1, 0 },
	{ 0, -1, 0 },

	{ -1, 0, 0 }, // LEFT
	{ -1, 0, 0 },
	{ -1, 0, 0 },
	{ -1, 0, 0 },

	{ 0, 1, 0 }, // UP
	{ 0, 1, 0 },
	{ 0, 1, 0 },
	{ 0, 1, 0 }
	};
	float tangents[24][3] = {
	{ 1, 0, 0 }, // BACK
	{ 1, 0, 0 },
	{ 1, 0, 0 },
	{ 1, 0, 0 },

	{ -1, 0, 0 }, // FRONT
	{ -1, 0, 0 },
	{ -1, 0, 0 },
	{ -1, 0, 0 },

	{ 0, 0, 1 }, // RIGHT
	{ 0, 0, 1 },
	{ 0, 0, 1 },
	{ 0, 0, 1 },

	{ 1, 0, 0 }, // DOWN
	{ 1, 0, 0 },
	{ 1, 0, 0 },
	{ 1, 0, 0 },

	{ 0, 0, -1 }, // LEFT
	{ 0, 0, -1 },
	{ 0, 0, -1 },
	{ 0, 0, -1 },

	{ 1, 0, 0 }, // UP
	{ 1, 0, 0 },
	{ 1, 0, 0 },
	{ 1, 0, 0 }
	};

	std::vector<Vertex> vertexList;
	vertexList.reserve(24);

	vertexList.emplace_back(pos[0], emptyColor, uv[0], normals[0], tangents[0]);
	vertexList.emplace_back(pos[1], emptyColor, uv[1], normals[1], tangents[1]);
	vertexList.emplace_back(pos[2], emptyColor, uv[2], normals[2], tangents[2]);
	vertexList.emplace_back(pos[3], emptyColor, uv[3], normals[3], tangents[3]);
	vertexList.emplace_back(pos[4], emptyColor, uv[4], normals[4], tangents[4]);
	vertexList.emplace_back(pos[5], emptyColor, uv[5], normals[5], tangents[5]);
	vertexList.emplace_back(pos[6], emptyColor, uv[6], normals[6], tangents[6]);
	vertexList.emplace_back(pos[7], emptyColor, uv[7], normals[7], tangents[7]);
	vertexList.emplace_back(pos[8], emptyColor, uv[8], normals[8], tangents[8]);
	vertexList.emplace_back(pos[9], emptyColor, uv[9], normals[9], tangents[9]);
	vertexList.emplace_back(pos[10], emptyColor, uv[10], normals[10], tangents[10]);
	vertexList.emplace_back(pos[11], emptyColor, uv[11], normals[11], tangents[11]);
	vertexList.emplace_back(pos[12], emptyColor, uv[12], normals[12], tangents[12]);
	vertexList.emplace_back(pos[13], emptyColor, uv[13], normals[13], tangents[13]);
	vertexList.emplace_back(pos[14], emptyColor, uv[14], normals[14], tangents[14]);
	vertexList.emplace_back(pos[15], emptyColor, uv[15], normals[15], tangents[15]);
	vertexList.emplace_back(pos[16], emptyColor, uv[16], normals[16], tangents[16]);
	vertexList.emplace_back(pos[17], emptyColor, uv[17], normals[17], tangents[17]);
	vertexList.emplace_back(pos[18], emptyColor, uv[18], normals[18], tangents[18]);
	vertexList.emplace_back(pos[19], emptyColor, uv[19], normals[19], tangents[19]);
	vertexList.emplace_back(pos[20], emptyColor, uv[20], normals[20], tangents[20]);
	vertexList.emplace_back(pos[21], emptyColor, uv[21], normals[21], tangents[21]);
	vertexList.emplace_back(pos[22], emptyColor, uv[22], normals[22], tangents[22]);
	vertexList.emplace_back(pos[23], emptyColor, uv[23], normals[23], tangents[23]);

	std::vector<unsigned> indexList = {
		0,1,2,
		0,2,3,
		4,5,6,
		4,6,7,
		8,9,10,
		8,10,11,
		12,13,14,
		12,14,15,
		16,17,18,
		16,18,19,
		20,21,22,
		20,22,23
	};

	std::vector<Mesh::Element> elementList;
	Mesh::Element& element = elementList.emplace_back();
	element.VertexOffset = 0;
	element.IndexOffset = 0;
	element.NumVertices = static_cast<unsigned>(vertexList.size());
	element.NumIndices = static_cast<unsigned>(indexList.size());

	Mesh cube;
	cube.InitBoundingBox({ -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f });
	cube.Initialize(std::move(vertexList), std::move(indexList), std::move(elementList), Mesh::Skeleton());
	return cube;
}

Mesh GraphicsEngine::CreateRampPrimitive()
{
	float emptyColor[4][4] = { {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
	float uv[18][4][2] = { {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

						   {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

						   {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

						   {{1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},

						   {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
						   {{0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 1.0f}},
	};

	float pos[18][4] = {
		{-1, 1, 1, 1}, // LEFT
		{-1, 1, -1, 1},
		{-1, -1, -1, 1},
		{-1, -1, 1, 1},

		{1, -1, -1, 1}, // DOWN
		{1, -1, 1, 1},
		{-1, -1, 1, 1},
		{-1, -1, -1, 1},

		{-1, 1, 1, 1}, // DIAGONAL
		{1, -1, 1, 1},
		{1, -1, -1, 1},
		{-1, 1, -1, 1},

		{-1, 1, 1, 1}, // FRONT
		{-1, -1, 1, 1},
		{1, -1, 1, 1},

		{1, -1, -1, 1}, // BACK
		{-1, -1, -1, 1},
		{ -1, 1, -1, 1 },
	};

	float normals[18][3] = {
		{ -1, 0, 0 }, // LEFT
		{ -1, 0, 0 },
		{ -1, 0, 0 },
		{ -1, 0, 0 },

		{ 0, -1, 0 }, // DOWN
		{ 0, -1, 0 },
		{ 0, -1, 0 },
		{ 0, -1, 0 },

		{ 1, 1, 0 }, // DIAGONAL
		{ 1, 1, 0 },
		{ 1, 1, 0 },
		{ 1, 1, 0 },

		{ 0, 0, 1 }, // FRONT
		{ 0, 0, 1 },
		{ 0, 0, 1 },

		{ 0, 0, -1 }, // BACK
		{ 0, 0, -1 },
		{ 0, 0, -1 }
	};

	float tangents[18][3] = {
		{ 0, 0, -1 }, // LEFT
		{ 0, 0, -1 },
		{ 0, 0, -1 },
		{ 0, 0, -1 },

		{ 0, 0, 1 }, // DOWN
		{ 0, 0, 1 },
		{ 0, 0, 1 },
		{ 0, 0, 1 },

		{ 0, 0, 1 }, // DIAGONAL
		{ 0, 0, 1 },
		{ 0, 0, 1 },
		{ 0, 0, 1 },

		{ -1, 0, 0 }, // FRONT
		{ -1, 0, 0 },
		{ -1, 0, 0 },

		{ 1, 0, 0 }, // BACK
		{ 1, 0, 0 },
		{ 1, 0, 0 }
	};

	std::vector<Vertex> vertexList;
	vertexList.reserve(18);

	vertexList.emplace_back(pos[0], emptyColor, uv[0], normals[0], tangents[0]);
	vertexList.emplace_back(pos[1], emptyColor, uv[1], normals[1], tangents[1]);
	vertexList.emplace_back(pos[2], emptyColor, uv[2], normals[2], tangents[2]);
	vertexList.emplace_back(pos[3], emptyColor, uv[3], normals[3], tangents[3]);
	vertexList.emplace_back(pos[4], emptyColor, uv[4], normals[4], tangents[4]);
	vertexList.emplace_back(pos[5], emptyColor, uv[5], normals[5], tangents[5]);
	vertexList.emplace_back(pos[6], emptyColor, uv[6], normals[6], tangents[6]);
	vertexList.emplace_back(pos[7], emptyColor, uv[7], normals[7], tangents[7]);
	vertexList.emplace_back(pos[8], emptyColor, uv[8], normals[8], tangents[8]);
	vertexList.emplace_back(pos[9], emptyColor, uv[9], normals[9], tangents[9]);
	vertexList.emplace_back(pos[10], emptyColor, uv[10], normals[10], tangents[10]);
	vertexList.emplace_back(pos[11], emptyColor, uv[11], normals[11], tangents[11]);
	vertexList.emplace_back(pos[12], emptyColor, uv[12], normals[12], tangents[12]);
	vertexList.emplace_back(pos[13], emptyColor, uv[13], normals[13], tangents[13]);
	vertexList.emplace_back(pos[14], emptyColor, uv[14], normals[14], tangents[14]);
	vertexList.emplace_back(pos[15], emptyColor, uv[15], normals[15], tangents[15]);
	vertexList.emplace_back(pos[16], emptyColor, uv[16], normals[16], tangents[16]);
	vertexList.emplace_back(pos[17], emptyColor, uv[17], normals[17], tangents[17]);

	std::vector<unsigned> indexList = {
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		6, 7, 4,

		8, 9, 10,
		10, 11, 8,

		12, 13, 14,
		15, 16, 17,
	};

	std::vector<Mesh::Element> elementList;
	Mesh::Element& element = elementList.emplace_back();
	element.VertexOffset = 0;
	element.IndexOffset = 0;
	element.NumVertices = static_cast<unsigned>(vertexList.size());
	element.NumIndices = static_cast<unsigned>(indexList.size());

	Mesh ramp;
	ramp.InitBoundingBox({ -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f });
	ramp.Initialize(std::move(vertexList), std::move(indexList), std::move(elementList), Mesh::Skeleton());
	return ramp;
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

	ConstantBuffer postProcessBuffer;
	myRHI->CreateConstantBuffer("PostProcessBuffer", sizeof(PostProcessBuffer), 7, PIPELINE_STAGE_PIXEL_SHADER, postProcessBuffer);
	myConstantBuffers.emplace(ConstantBufferType::PostProcessBuffer, std::move(postProcessBuffer));
}
