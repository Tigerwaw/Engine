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
	myRHI = std::make_shared<RenderHardwareInterface>();
	myPostProcessingSettings = std::make_unique<PostProcessingSettings>();
	myDrawer = std::make_unique<Drawer>();
	myResourceVendor = std::make_unique<ResourceVendor>();

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

	if (!myResourceVendor->Initialize(myRHI))
	{
		myResourceVendor = nullptr;
		LOG(LogGraphicsEngine, Error, "Failed to initialize Resource Vendor!");
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

	myBRDFLUTTexture = std::make_shared<Texture>();
	rhi.CreateLUT("LUT", 512, 512, myBRDFLUTTexture, aContentRoot / L"EngineAssets/Shaders/Quad_VS.cso", aContentRoot / L"EngineAssets/Shaders/brdfLUT_PS.cso");

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

void GraphicsEngine::SetRenderTarget(std::shared_ptr<Texture> aRenderTarget, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget, bool aClearDepthStencil)
{
	myRHI->SetRenderTarget(aRenderTarget, aDepthStencil, aClearRenderTarget, aClearDepthStencil);
}

void GraphicsEngine::SetRenderTargets(const std::vector<std::shared_ptr<Texture>>& aRenderTargets, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget, bool aClearDepthStencil)
{
	myRHI->SetRenderTargets(aRenderTargets, aDepthStencil, aClearRenderTarget, aClearDepthStencil);
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
