#include "GraphicsEngine.pch.h"
#include "GraphicsEngine.h"

#include "ShaderReflection/ShaderInfo.h"

#include "Engine.h"
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
#include "AssetManager.h"

bool GraphicsEngine::Initialize(HWND aWindowHandle)
{
	myRHI = std::make_unique<RenderHardwareInterface>();

	LOG(LogGraphicsEngine, Log, "Initializing Graphics Engine...");

	bool createDebugLayer = false;

#ifdef _DEBUG
	createDebugLayer = true;
#endif

	RenderHardwareInterface& rhi = *myRHI;
	if (!rhi.Initialize(aWindowHandle, createDebugLayer))
	{
		myRHI.reset();
		LOG(LogGraphicsEngine, Error, "Failed to initialize graphics engine!");
		return false;
	}

	{
		PipelineStateObject defaultPSO;
		defaultPSO.SamplerStates[0] = rhi.GetSamplerState("LinearWrapSS");

		defaultPSO.VertexStride = sizeof(Vertex);

		std::wstring root = Engine::Get().GetContentRootPath().wstring();
		if (!rhi.CreateInputLayout(defaultPSO.InputLayout, Vertex::InputLayoutDefinition, root + L"EngineAssets/Shaders/SH_Mesh_VS.cso"))
		{
			LOG(LogGraphicsEngine, Error, "Failed to load default input layout!");
			return false;
		}

		defaultPSO.VertexShader = std::make_shared<Shader>();
		if (!rhi.LoadShaderFromFilePath("Default_VS", *defaultPSO.VertexShader, root + L"EngineAssets/Shaders/SH_Mesh_VS.cso"))
		{
			LOG(LogGraphicsEngine, Error, "Failed to load default vertex shader!");
			return false;
		}

		defaultPSO.PixelShader = std::make_shared<Shader>();
		if (!rhi.LoadShaderFromFilePath("Default_PS", *defaultPSO.PixelShader, root + L"EngineAssets/Shaders/SH_PBRMesh_PS.cso"))
		{
			LOG(LogGraphicsEngine, Error, "Failed to load default pixel shader!");
			return false;
		}

		myDefaultPSO = std::make_shared<PipelineStateObject>(defaultPSO);
		myCurrentPSO = myDefaultPSO;
	}

	myLUTtexture = std::make_shared<Texture>();
	rhi.CreateLUT("LUT", 512, 512, myLUTtexture);

	CreateConstantBuffers();
	
	myCommandList = std::make_unique<GraphicsCommandList>();
	myGBuffer = std::make_unique<GBuffer>();
	Math::Vector2f resolution = Engine::Get().GetResolution();
	myGBuffer->CreateGBuffer(static_cast<unsigned>(resolution.x), static_cast<unsigned>(resolution.y));

	CreateRandomKernel(64);

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
	ChangePipelineState(myCurrentPSO);
}

void GraphicsEngine::RenderFrame()
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "Execute Render Command List");
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

			for (auto& [slot, texture] : aMaterialList[element.MaterialIndex]->GetTextures())
			{
				SetTextureResource_PS(slot, *texture);
			}
		}

		myRHI->DrawIndexed(element.IndexOffset, element.NumIndices);
		myDrawcallAmount++;

		for (auto& [slot, texture] : aMaterialList[element.MaterialIndex]->GetTextures())
		{
			ClearTextureResource_PS(slot);
		}
	}

	ClearTextureResource_PS(127);
}

void GraphicsEngine::RenderInstancedMesh(const Mesh& aMesh, unsigned aMeshCount, std::vector<std::shared_ptr<Material>> aMaterialList, DynamicVertexBuffer& aInstanceBuffer, bool aOverrideMaterialPSO)
{
	std::vector<ID3D11Buffer*> buffers;
	std::vector<unsigned> strides;
	std::vector<unsigned> offsets;

	buffers.emplace_back(*aMesh.GetVertexBuffer().GetAddressOf());
	buffers.emplace_back(*aInstanceBuffer.GetVertexBuffer().GetAddressOf());

	strides.emplace_back(myCurrentPSO->VertexStride);
	strides.emplace_back(static_cast<unsigned>(sizeof(Math::Matrix4x4f)));

	offsets.emplace_back(0);
	offsets.emplace_back(0);

	myRHI->SetVertexBuffers(buffers, strides, offsets);
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

			for (auto& [slot, texture] : aMaterialList[element.MaterialIndex]->GetTextures())
			{
				SetTextureResource_PS(slot, *texture);
			}
		}

		myRHI->DrawIndexedInstanced(element.NumIndices, aMeshCount, element.IndexOffset, 0, 0);
		myDrawcallAmount++;

		for (auto& [slot, texture] : aMaterialList[element.MaterialIndex]->GetTextures())
		{
			ClearTextureResource_PS(slot);
		}
	}

	ClearTextureResource_PS(127);
}

void GraphicsEngine::RenderSprite()
{
	myRHI->SetPrimitiveTopology(Topology::POINTLIST);
	myRHI->Draw(1);
}

void GraphicsEngine::RenderText(const Text& aText)
{
	SetTextureResource_PS(10, *aText.GetTexture());

	const Text::TextData& textData = aText.GetTextData();
	myRHI->SetVertexBuffer(textData.vertexBuffer->GetVertexBuffer(), myCurrentPSO->VertexStride, 0);
	myRHI->SetIndexBuffer(textData.indexBuffer);
	myRHI->SetPrimitiveTopology(Topology::TRIANGLELIST);

	myRHI->DrawIndexed(0, textData.numIndices);
	myDrawcallAmount++;

	ClearTextureResource_PS(10);
}

void GraphicsEngine::RenderDebugLines(DynamicVertexBuffer& aDynamicBuffer, unsigned aLineAmount)
{
	myRHI->SetVertexBuffer(aDynamicBuffer.GetVertexBuffer(), myCurrentPSO->VertexStride, 0);
	myRHI->SetPrimitiveTopology(Topology::POINTLIST);
	myRHI->Draw(aLineAmount);
}

void GraphicsEngine::RenderParticleEmitter(ParticleEmitter& aParticleEmitter)
{
	ChangePipelineState(aParticleEmitter.GetMaterial()->GetPSO());

	for (const auto& [slot, texture] : aParticleEmitter.GetMaterial()->GetTextures())
	{
		SetTextureResource_PS(slot, *texture);
	}

	myRHI->SetPrimitiveTopology(Topology::POINTLIST);
	myRHI->SetVertexBuffer(aParticleEmitter.myVertexBuffer.GetVertexBuffer(), myCurrentPSO->VertexStride, 0);
	myRHI->Draw(static_cast<unsigned>(aParticleEmitter.myParticles.size()));

	for (const auto& [slot, texture] : aParticleEmitter.GetMaterial()->GetTextures())
	{
		ClearTextureResource_PS(slot);
	}
}

void GraphicsEngine::RenderTrailEmitter(TrailEmitter& aTrailEmitter)
{
	ChangePipelineState(aTrailEmitter.GetMaterial()->GetPSO());

	for (const auto& [slot, texture] : aTrailEmitter.GetMaterial()->GetTextures())
	{
		SetTextureResource_PS(slot, *texture);
	}

	myRHI->SetPrimitiveTopology(Topology::LINESTRIP);
	myRHI->SetVertexBuffer(aTrailEmitter.myVertexBuffer.GetVertexBuffer(), myCurrentPSO->VertexStride, 0);
	unsigned count = aTrailEmitter.GetCurrentLength();
	myRHI->Draw(count);

	for (const auto& [slot, texture] : aTrailEmitter.GetMaterial()->GetTextures())
	{
		ClearTextureResource_PS(slot);
	}
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

void GraphicsEngine::CreateRandomKernel(unsigned aKernelSize)
{
	myRandomKernel.resize(aKernelSize);

	std::uniform_real_distribution<float> randomValues(0, 1);
	std::default_random_engine rng;

	for (unsigned i = 0; i < aKernelSize; ++i)
	{
		Math::Vector4f v = {
		randomValues(rng) * 2.0f - 1.0f,
		randomValues(rng) * 2.0f - 1.0f,
		randomValues(rng),
		0
		};
		v = v.GetNormalized();
		float s = static_cast<float>(i) / static_cast<float>(aKernelSize);
		s = std::lerp(0.1f, 1.0f, s * s);
		v *= s;
		myRandomKernel[i] = v;
	}
}
