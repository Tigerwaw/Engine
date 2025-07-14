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

#include "DefaultTextures/Default_C.h"
#include "DefaultTextures/Default_N.h"
#include "DefaultTextures/Default_M.h"
#include "DefaultTextures/Default_FX.h"

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

	if (!CreateConstantBuffers())
	{
		LOG(LogGraphicsEngine, Error, "Failed to create constant buffers!");
		return false;
	}

	if (!CreateDefaultTextures(aContentRoot))
	{
		LOG(LogGraphicsEngine, Error, "Failed to create default textures!");
		return false;
	}

	if (!CreateDefaultPSOs(aContentRoot))
	{
		LOG(LogGraphicsEngine, Error, "Failed to create default PSO!");
		return false;
	}

	if (!CreateMeshPrimitives())
	{
		LOG(LogGraphicsEngine, Error, "Failed to create mesh primitives!");
		return false;
	}

	ChangePipelineState(myDefaultPSO);
	myDefaultMaterial = std::make_shared<Material>();
	myDefaultMaterial->SetPSO(myDefaultPSO);
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

bool GraphicsEngine::HasPSO(const char* aPSOName) const
{
	return myPSOs.contains(aPSOName);
}

std::shared_ptr<PipelineStateObject> GraphicsEngine::GetPSO(const char* aPSOName) const
{
	if (HasPSO(aPSOName))
	{
		return myPSOs.at(aPSOName);
	}

	return std::shared_ptr<PipelineStateObject>();
}

std::shared_ptr<PipelineStateObject> GraphicsEngine::GetPSO(PSOType aPSOType) const
{
	if (HasPSO(PSOName(aPSOType)))
	{
		return myPSOs.at(PSOName(aPSOType));
	}

	return std::shared_ptr<PipelineStateObject>();
}

std::shared_ptr<PipelineStateObject> GraphicsEngine::RegisterPSO(const char* aPSOName, std::shared_ptr<PipelineStateObject> aPSO)
{
	myPSOs[aPSOName] = aPSO;
	return aPSO;
}

GraphicsEngine::GraphicsEngine() = default;
GraphicsEngine::~GraphicsEngine() = default;

bool GraphicsEngine::CreateMeshPrimitives()
{
	ResourceVendor& rv = GetResourceVendor();
	myPlanePrimitive = std::make_shared<Mesh>(rv.CreatePlanePrimitive());
	myCubePrimitive = std::make_shared<Mesh>(rv.CreateCubePrimitive());
	myRampPrimitive = std::make_shared<Mesh>(rv.CreateRampPrimitive());

	return true;
}

bool GraphicsEngine::CreateDefaultPSOs(const std::filesystem::path& aContentRoot)
{
	ResourceVendor& rv = GetResourceVendor();
	std::filesystem::path defaultMeshVSPath = aContentRoot / "EngineAssets/Shaders/Mesh_VS.cso";
	std::shared_ptr<Shader> defaultMeshVS = std::make_shared<Shader>();
	if (!rv.LoadShader(defaultMeshVSPath, *defaultMeshVS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", defaultMeshVSPath.filename().string());
		return false;
	}

	std::filesystem::path deferredMeshPSPath = aContentRoot / "EngineAssets/Shaders/Deferred_PS.cso";
	std::shared_ptr<Shader> deferredMeshPS = std::make_shared<Shader>();
	if (!rv.LoadShader(deferredMeshPSPath, *deferredMeshPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", deferredMeshPSPath.filename().string());
		return false;
	}

	std::filesystem::path forwardPBRMeshPSPath = aContentRoot / "EngineAssets/Shaders/PBRMesh_PS.cso";
	std::shared_ptr<Shader> forwardPBRMeshPS = std::make_shared<Shader>();
	if (!rv.LoadShader(forwardPBRMeshPSPath, *forwardPBRMeshPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", forwardPBRMeshPSPath.filename().string());
		return false;
	}

	std::filesystem::path shadowCubeVSPath = aContentRoot / "EngineAssets/Shaders/ShadowCube_VS.cso";
	std::shared_ptr<Shader> shadowCubeVS = std::make_shared<Shader>();
	if (!rv.LoadShader(shadowCubeVSPath, *shadowCubeVS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", shadowCubeVSPath.filename().string());
		return false;
	}

	std::filesystem::path shadowCubeGSPath = aContentRoot / "EngineAssets/Shaders/ShadowCube_GS.cso";
	std::shared_ptr<Shader> shadowCubeGS = std::make_shared<Shader>();
	if (!rv.LoadShader(shadowCubeGSPath, *shadowCubeGS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", shadowCubeGSPath.filename().string());
		return false;
	}

	std::filesystem::path spriteVSPath = aContentRoot / "EngineAssets/Shaders/Sprite_VS.cso";
	std::shared_ptr<Shader> spriteVS = std::make_shared<Shader>();
	if (!rv.LoadShader(spriteVSPath, *spriteVS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", spriteVSPath.filename().string());
		return false;
	}

	std::filesystem::path spriteGSPath = aContentRoot / "EngineAssets/Shaders/Sprite_GS.cso";
	std::shared_ptr<Shader> spriteGS = std::make_shared<Shader>();
	if (!rv.LoadShader(spriteGSPath, *spriteGS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", spriteGSPath.filename().string());
		return false;
	}

	std::filesystem::path spritePSPath = aContentRoot / "EngineAssets/Shaders/Sprite_PS.cso";
	std::shared_ptr<Shader> spritePS = std::make_shared<Shader>();
	if (!rv.LoadShader(spritePSPath, *spritePS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", spritePSPath.filename().string());
		return false;
	}

	std::filesystem::path spritesheetPSPath = aContentRoot / "EngineAssets/Shaders/Spritesheet_PS.cso";
	std::shared_ptr<Shader> spritesheetPS = std::make_shared<Shader>();
	if (!rv.LoadShader(spritesheetPSPath, *spritesheetPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", spritesheetPSPath.filename().string());
		return false;
	}

	std::filesystem::path textVSPath = aContentRoot / "EngineAssets/Shaders/UI_Text_VS.cso";
	std::shared_ptr<Shader> textVS = std::make_shared<Shader>();
	if (!rv.LoadShader(textVSPath, *textVS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", textVSPath.filename().string());
		return false;
	}

	std::filesystem::path textPSPath = aContentRoot / "EngineAssets/Shaders/UI_Text_PS.cso";
	std::shared_ptr<Shader> textPS = std::make_shared<Shader>();
	if (!rv.LoadShader(textPSPath, *textPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", textPSPath.filename().string());
		return false;
	}

	std::filesystem::path particleVSPath = aContentRoot / "EngineAssets/Shaders/Particle_VS.cso";
	std::shared_ptr<Shader> particleVS = std::make_shared<Shader>();
	if (!rv.LoadShader(particleVSPath, *particleVS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", particleVSPath.filename().string());
		return false;
	}

	std::filesystem::path particleGSPath = aContentRoot / "EngineAssets/Shaders/Particle_GS.cso";
	std::shared_ptr<Shader> particleGS = std::make_shared<Shader>();
	if (!rv.LoadShader(particleGSPath, *particleGS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", particleGSPath.filename().string());
		return false;
	}

	std::filesystem::path particlePSPath = aContentRoot / "EngineAssets/Shaders/Particle_PS.cso";
	std::shared_ptr<Shader> particlePS = std::make_shared<Shader>();
	if (!rv.LoadShader(particlePSPath, *particlePS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", particlePSPath.filename().string());
		return false;
	}

	std::filesystem::path trailVSPath = aContentRoot / "EngineAssets/Shaders/Trail_VS.cso";
	std::shared_ptr<Shader> trailVS = std::make_shared<Shader>();
	if (!rv.LoadShader(trailVSPath, *trailVS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", particleVSPath.filename().string());
		return false;
	}

	std::filesystem::path trailGSPath = aContentRoot / "EngineAssets/Shaders/Trail_GS.cso";
	std::shared_ptr<Shader> trailGS = std::make_shared<Shader>();
	if (!rv.LoadShader(trailGSPath, *trailGS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", trailGSPath.filename().string());
		return false;
	}

	std::filesystem::path trailPSPath = aContentRoot / "EngineAssets/Shaders/Trail_PS.cso";
	std::shared_ptr<Shader> trailPS = std::make_shared<Shader>();
	if (!rv.LoadShader(trailPSPath, *trailPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", trailPSPath.filename().string());
		return false;
	}

	std::filesystem::path particleSpritesheetPSPath = aContentRoot / "EngineAssets/Shaders/ParticleSpritesheet_PS.cso";
	std::shared_ptr<Shader> particleSpritesheetPS = std::make_shared<Shader>();
	if (!rv.LoadShader(particleSpritesheetPSPath, *particleSpritesheetPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", particleSpritesheetPSPath.filename().string());
		return false;
	}

	std::filesystem::path vfxSpritesheetPSPath = aContentRoot / "EngineAssets/Shaders/VFXSpritesheet_PS.cso";
	std::shared_ptr<Shader> vfxSpritesheetPS = std::make_shared<Shader>();
	if (!rv.LoadShader(vfxSpritesheetPSPath, *vfxSpritesheetPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", vfxSpritesheetPSPath.filename().string());
		return false;
	}

	std::filesystem::path debugLineVSPath = aContentRoot / "EngineAssets/Shaders/DebugLine_VS.cso";
	std::shared_ptr<Shader> debugLineVS = std::make_shared<Shader>();
	if (!rv.LoadShader(debugLineVSPath, *debugLineVS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", debugLineVSPath.filename().string());
		return false;
	}

	std::filesystem::path debugLineGSPath = aContentRoot / "EngineAssets/Shaders/DebugLine_GS.cso";
	std::shared_ptr<Shader> debugLineGS = std::make_shared<Shader>();
	if (!rv.LoadShader(debugLineGSPath, *debugLineGS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", debugLineGSPath.filename().string());
		return false;
	}
	
	std::filesystem::path debugLinePSPath = aContentRoot / "EngineAssets/Shaders/DebugObject_PS.cso";
	std::shared_ptr<Shader> debugLinePS = std::make_shared<Shader>();
	if (!rv.LoadShader(debugLinePSPath, *debugLinePS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", debugLinePSPath.filename().string());
		return false;
	}
	
	std::filesystem::path quadVSPath = aContentRoot / "EngineAssets/Shaders/Quad_VS.cso";
	std::shared_ptr<Shader> quadVS = std::make_shared<Shader>();
	if (!rv.LoadShader(quadVSPath, *quadVS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", quadVSPath.filename().string());
		return false;
	}
	
	std::filesystem::path deferredDirectionalLightPSPath = aContentRoot / "EngineAssets/Shaders/Deferred_DirectionalLight_PS.cso";
	std::shared_ptr<Shader> deferredDirectionalLightPS = std::make_shared<Shader>();
	if (!rv.LoadShader(deferredDirectionalLightPSPath, *deferredDirectionalLightPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", deferredDirectionalLightPSPath.filename().string());
		return false;
	}
	
	std::filesystem::path deferredPointlightPSPath = aContentRoot / "EngineAssets/Shaders/Deferred_Pointlight_PS.cso";
	std::shared_ptr<Shader> deferredPointlightPS = std::make_shared<Shader>();
	if (!rv.LoadShader(deferredPointlightPSPath, *deferredPointlightPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", deferredPointlightPSPath.filename().string());
		return false;
	}
	
	std::filesystem::path deferredSpotlightPSPath = aContentRoot / "EngineAssets/Shaders/Deferred_Spotlight_PS.cso";
	std::shared_ptr<Shader> deferredSpotlightPS = std::make_shared<Shader>();
	if (!rv.LoadShader(deferredSpotlightPSPath, *deferredSpotlightPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", deferredSpotlightPSPath.filename().string());
		return false;
	}
	
	std::filesystem::path bloomPSPath = aContentRoot / "EngineAssets/Shaders/PP_Bloom_PS.cso";
	std::shared_ptr<Shader> bloomPS = std::make_shared<Shader>();
	if (!rv.LoadShader(bloomPSPath, *bloomPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", bloomPSPath.filename().string());
		return false;
	}
	
	std::filesystem::path gaussianhPSPath = aContentRoot / "EngineAssets/Shaders/PP_GaussianH_PS.cso";
	std::shared_ptr<Shader> gaussianhPS = std::make_shared<Shader>();
	if (!rv.LoadShader(gaussianhPSPath, *gaussianhPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", gaussianhPSPath.filename().string());
		return false;
	}
	
	std::filesystem::path gaussianvPSPath = aContentRoot / "EngineAssets/Shaders/PP_GaussianV_PS.cso";
	std::shared_ptr<Shader> gaussianvPS = std::make_shared<Shader>();
	if (!rv.LoadShader(gaussianvPSPath, *gaussianvPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", gaussianhPSPath.filename().string());
		return false;
	}
	
	std::filesystem::path luminancePSPath = aContentRoot / "EngineAssets/Shaders/PP_Luminance_PS.cso";
	std::shared_ptr<Shader> luminancePS = std::make_shared<Shader>();
	if (!rv.LoadShader(luminancePSPath, *luminancePS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", luminancePSPath.filename().string());
		return false;
	}
	
	std::filesystem::path radialBlurPSPath = aContentRoot / "EngineAssets/Shaders/PP_RadialBlur_PS.cso";
	std::shared_ptr<Shader> radialBlurPS = std::make_shared<Shader>();
	if (!rv.LoadShader(radialBlurPSPath, *radialBlurPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", radialBlurPSPath.filename().string());
		return false;
	}
	
	std::filesystem::path resamplePSPath = aContentRoot / "EngineAssets/Shaders/PP_Resample_PS.cso";
	std::shared_ptr<Shader> resamplePS = std::make_shared<Shader>();
	if (!rv.LoadShader(resamplePSPath, *resamplePS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", resamplePSPath.filename().string());
		return false;
	}
	
	std::filesystem::path ssaoPSPath = aContentRoot / "EngineAssets/Shaders/PP_SSAO_PS.cso";
	std::shared_ptr<Shader> ssaoPS = std::make_shared<Shader>();
	if (!rv.LoadShader(ssaoPSPath, *ssaoPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", ssaoPSPath.filename().string());
		return false;
	}
	
	std::filesystem::path tonemapACESPSPath = aContentRoot / "EngineAssets/Shaders/PP_TonemapACES_PS.cso";
	std::shared_ptr<Shader> tonemapACESPS = std::make_shared<Shader>();
	if (!rv.LoadShader(tonemapACESPSPath, *tonemapACESPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", tonemapACESPSPath.filename().string());
		return false;
	}
	
	std::filesystem::path tonemapLottesPSPath = aContentRoot / "EngineAssets/Shaders/PP_TonemapLottes_PS.cso";
	std::shared_ptr<Shader> tonemapLottesPS = std::make_shared<Shader>();
	if (!rv.LoadShader(tonemapLottesPSPath, *tonemapLottesPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", tonemapLottesPSPath.filename().string());
		return false;
	}
	
	std::filesystem::path tonemapUEPSPath = aContentRoot / "EngineAssets/Shaders/PP_TonemapUE_PS.cso";
	std::shared_ptr<Shader> tonemapUEPS = std::make_shared<Shader>();
	if (!rv.LoadShader(tonemapUEPSPath, *tonemapUEPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", tonemapUEPSPath.filename().string());
		return false;
	}
	
	std::filesystem::path wireframePSPath = aContentRoot / "EngineAssets/Shaders/Wireframe_PS.cso";
	std::shared_ptr<Shader> wireframePS = std::make_shared<Shader>();
	if (!rv.LoadShader(wireframePSPath, *wireframePS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", wireframePSPath.filename().string());
		return false;
	}
	
	std::filesystem::path debugAOPSPath = aContentRoot / "EngineAssets/Shaders/DebugAO_PS.cso";
	std::shared_ptr<Shader> debugAOPS = std::make_shared<Shader>();
	if (!rv.LoadShader(debugAOPSPath, *debugAOPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", debugAOPSPath.filename().string());
		return false;
	}
	
	std::filesystem::path debugRoughnessPSPath = aContentRoot / "EngineAssets/Shaders/DebugRoughness_PS.cso";
	std::shared_ptr<Shader> debugRoughnessPS = std::make_shared<Shader>();
	if (!rv.LoadShader(debugRoughnessPSPath, *debugRoughnessPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", debugRoughnessPSPath.filename().string());
		return false;
	}
	
	std::filesystem::path debugMetallicPSPath = aContentRoot / "EngineAssets/Shaders/DebugMetallic_PS.cso";
	std::shared_ptr<Shader> debugMetallicPS = std::make_shared<Shader>();
	if (!rv.LoadShader(debugMetallicPSPath, *debugMetallicPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", debugMetallicPSPath.filename().string());
		return false;
	}
	
	std::filesystem::path debugPixelNormalsPSPath = aContentRoot / "EngineAssets/Shaders/DebugPixelNormals_PS.cso";
	std::shared_ptr<Shader> debugPixelNormalsPS = std::make_shared<Shader>();
	if (!rv.LoadShader(debugPixelNormalsPSPath, *debugPixelNormalsPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", debugPixelNormalsPSPath.filename().string());
		return false;
	}
	
	std::filesystem::path debugFXPSPath = aContentRoot / "EngineAssets/Shaders/DebugFX_PS.cso";
	std::shared_ptr<Shader> debugFXPS = std::make_shared<Shader>();
	if (!rv.LoadShader(debugFXPSPath, *debugFXPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", debugFXPSPath.filename().string());
		return false;
	}
	
	std::filesystem::path debugUVsPSPath = aContentRoot / "EngineAssets/Shaders/DebugUVs_PS.cso";
	std::shared_ptr<Shader> debugUVsPS = std::make_shared<Shader>();
	if (!rv.LoadShader(debugUVsPSPath, *debugUVsPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", debugUVsPSPath.filename().string());
		return false;
	}
	
	std::filesystem::path debugVertexColorPSPath = aContentRoot / "EngineAssets/Shaders/DebugVertexColor_PS.cso";
	std::shared_ptr<Shader> debugVertexColorPS = std::make_shared<Shader>();
	if (!rv.LoadShader(debugVertexColorPSPath, *debugVertexColorPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", debugVertexColorPSPath.filename().string());
		return false;
	}
	
	std::filesystem::path debugVertexNormalsPSPath = aContentRoot / "EngineAssets/Shaders/DebugVertexNormals_PS.cso";
	std::shared_ptr<Shader> debugVertexNormalsPS = std::make_shared<Shader>();
	if (!rv.LoadShader(debugVertexNormalsPSPath, *debugVertexNormalsPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", debugVertexNormalsPSPath.filename().string());
		return false;
	}
	
	std::filesystem::path debugTextureNormalsPSPath = aContentRoot / "EngineAssets/Shaders/DebugTextureNormals_PS.cso";
	std::shared_ptr<Shader> debugTextureNormalsPS = std::make_shared<Shader>();
	if (!rv.LoadShader(debugTextureNormalsPSPath, *debugTextureNormalsPS))
	{
		LOG(LogGraphicsEngine, Error, "Failed to load shader {}", debugTextureNormalsPSPath.filename().string());
		return false;
	}

	{
		PSODescription deferredDesc;
		deferredDesc.name = PSOName(PSOType::Deferred);
		deferredDesc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		deferredDesc.vertexStride = sizeof(Vertex);
		deferredDesc.vsPath = defaultMeshVSPath;
		deferredDesc.vsShader = defaultMeshVS;
		deferredDesc.psShader = deferredMeshPS;
		deferredDesc.samplerList[0] = "LinearWrapSS";

		std::shared_ptr<PipelineStateObject> deferred = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*deferred, deferredDesc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", deferredDesc.name);
			return false;
		}

		RegisterPSO(deferredDesc.name.c_str(), deferred);
		myDefaultPSO = deferred;
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::ForwardPBR);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = defaultMeshVSPath;
		desc.vsShader = defaultMeshVS;
		desc.psShader = forwardPBRMeshPS;
		desc.samplerList[0] = "LinearWrapSS";
		desc.samplerList[14] = "LutSS";
		desc.samplerList[15] = "ShadowSS";

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::TransparentPBR);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = defaultMeshVSPath;
		desc.vsShader = defaultMeshVS;
		desc.psShader = forwardPBRMeshPS;
		desc.samplerList[0] = "LinearWrapSS";
		desc.samplerList[14] = "LutSS";
		desc.samplerList[15] = "ShadowSS";
		desc.blendMode = BlendMode::Alpha;

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}
	
	{
		PSODescription desc;
		desc.name = PSOName(PSOType::Shadow);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = defaultMeshVSPath;
		desc.vsShader = defaultMeshVS;

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::ShadowCube);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = shadowCubeVSPath;
		desc.vsShader = shadowCubeVS;
		desc.gsShader = shadowCubeGS;

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::Sprite);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = spriteVSPath;
		desc.vsShader = spriteVS;
		desc.gsShader = spriteGS;
		desc.psShader = spritePS;
		desc.samplerList[0] = "LinearWrapSS";

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::Spritesheet);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = spriteVSPath;
		desc.vsShader = spriteVS;
		desc.gsShader = spriteGS;
		desc.psShader = spritesheetPS;
		desc.samplerList[0] = "LinearWrapSS";

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::Text);
		desc.inputLayoutDefinition = TextVertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(TextVertex);
		desc.vsPath = textVSPath;
		desc.vsShader = textVS;
		desc.psShader = textPS;
		desc.samplerList[0] = "LinearWrapSS";

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::Particle);
		desc.inputLayoutDefinition = ParticleVertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(ParticleVertex);
		desc.vsPath = particleVSPath;
		desc.vsShader = particleVS;
		desc.gsShader = particleGS;
		desc.psShader = particlePS;
		desc.samplerList[0] = "LinearWrapSS";
		desc.blendMode = BlendMode::Alpha;
		desc.useReadOnlyDepthStencilState = true;

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::Trail);
		desc.inputLayoutDefinition = TrailVertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(TrailVertex);
		desc.vsPath = trailVSPath;
		desc.vsShader = trailVS;
		desc.gsShader = trailGS;
		desc.psShader = trailPS;
		desc.samplerList[0] = "LinearWrapSS";
		desc.fillMode = 3;
		desc.cullMode = 1;
		desc.blendMode = BlendMode::Alpha;
		desc.useReadOnlyDepthStencilState = true;

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::ParticleSpritesheet);
		desc.inputLayoutDefinition = ParticleVertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(ParticleVertex);
		desc.vsPath = particleVSPath;
		desc.vsShader = particleVS;
		desc.gsShader = particleGS;
		desc.psShader = particleSpritesheetPS;
		desc.samplerList[0] = "LinearWrapSS";
		desc.blendMode = BlendMode::Alpha;
		desc.useReadOnlyDepthStencilState = true;

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::VFXSpritesheet);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = defaultMeshVSPath;
		desc.vsShader = defaultMeshVS;
		desc.psShader = vfxSpritesheetPS;
		desc.samplerList[0] = "LinearWrapSS";
		desc.samplerList[14] = "LutSS";
		desc.samplerList[15] = "ShadowSS";
		desc.blendMode = BlendMode::Alpha;

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::DebugLine);
		desc.inputLayoutDefinition = DebugLineVertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(DebugLineVertex);
		desc.vsPath = debugLineVSPath;
		desc.vsShader = debugLineVS;
		desc.gsShader = debugLineGS;
		desc.psShader = debugLinePS;
		desc.fillMode = 3;
		desc.cullMode = 1;
		desc.antiAliasedLine = true;

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::DeferredDirectionalLight);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = quadVSPath;
		desc.vsShader = quadVS;
		desc.psShader = deferredDirectionalLightPS;
		desc.samplerList[0] = "LinearWrapSS";
		desc.samplerList[14] = "LutSS";
		desc.samplerList[15] = "ShadowSS";
		desc.blendMode = BlendMode::Additive;

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::DeferredPointlight);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = quadVSPath;
		desc.vsShader = quadVS;
		desc.psShader = deferredPointlightPS;
		desc.samplerList[0] = "LinearWrapSS";
		desc.samplerList[14] = "LutSS";
		desc.samplerList[15] = "ShadowSS";
		desc.blendMode = BlendMode::Additive;

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::DeferredSpotlight);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = quadVSPath;
		desc.vsShader = quadVS;
		desc.psShader = deferredSpotlightPS;
		desc.samplerList[0] = "LinearWrapSS";
		desc.samplerList[14] = "LutSS";
		desc.samplerList[15] = "ShadowSS";
		desc.blendMode = BlendMode::Additive;

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::Bloom);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = quadVSPath;
		desc.vsShader = quadVS;
		desc.psShader = bloomPS;
		desc.samplerList[0] = "LinearWrapSS";
		desc.blendMode = BlendMode::Alpha;

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::GaussianH);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = quadVSPath;
		desc.vsShader = quadVS;
		desc.psShader = gaussianhPS;
		desc.samplerList[1] = "LinearClampSS";

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::GaussianV);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = quadVSPath;
		desc.vsShader = quadVS;
		desc.psShader = gaussianvPS;
		desc.samplerList[1] = "LinearClampSS";

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::Luminance);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = quadVSPath;
		desc.vsShader = quadVS;
		desc.psShader = luminancePS;
		desc.samplerList[0] = "LinearWrapSS";

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::RadialBlur);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = quadVSPath;
		desc.vsShader = quadVS;
		desc.psShader = radialBlurPS;
		desc.samplerList[1] = "LinearClampSS";

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::Resample);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = quadVSPath;
		desc.vsShader = quadVS;
		desc.psShader = resamplePS;
		desc.samplerList[1] = "LinearClampSS";

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::SSAO);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = quadVSPath;
		desc.vsShader = quadVS;
		desc.psShader = ssaoPS;
		desc.samplerList[2] = "PointWrapSS";
		desc.samplerList[3] = "PointClampSS";
		desc.blendMode = BlendMode::Alpha;

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::TonemapACES);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = quadVSPath;
		desc.vsShader = quadVS;
		desc.psShader = tonemapACESPS;
		desc.samplerList[0] = "LinearWrapSS";

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::TonemapLottes);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = quadVSPath;
		desc.vsShader = quadVS;
		desc.psShader = tonemapLottesPS;
		desc.samplerList[0] = "LinearWrapSS";

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::TonemapUE);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = quadVSPath;
		desc.vsShader = quadVS;
		desc.psShader = tonemapUEPS;
		desc.samplerList[0] = "LinearWrapSS";

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::Wireframe);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = defaultMeshVSPath;
		desc.vsShader = defaultMeshVS;
		desc.psShader = wireframePS;
		desc.fillMode = 2;
		desc.cullMode = 1;

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::DebugAO);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = defaultMeshVSPath;
		desc.vsShader = defaultMeshVS;
		desc.psShader = debugAOPS;
		desc.samplerList[0] = "LinearWrapSS";

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::DebugAO);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = defaultMeshVSPath;
		desc.vsShader = defaultMeshVS;
		desc.psShader = debugAOPS;
		desc.samplerList[0] = "LinearWrapSS";

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::DebugRoughness);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = defaultMeshVSPath;
		desc.vsShader = defaultMeshVS;
		desc.psShader = debugAOPS;
		desc.samplerList[0] = "LinearWrapSS";

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::DebugMetallic);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = defaultMeshVSPath;
		desc.vsShader = defaultMeshVS;
		desc.psShader = debugMetallicPS;
		desc.samplerList[0] = "LinearWrapSS";

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::DebugPixelNormals);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = defaultMeshVSPath;
		desc.vsShader = defaultMeshVS;
		desc.psShader = debugPixelNormalsPS;
		desc.samplerList[0] = "LinearWrapSS";

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::DebugFX);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = defaultMeshVSPath;
		desc.vsShader = defaultMeshVS;
		desc.psShader = debugFXPS;
		desc.samplerList[0] = "LinearWrapSS";

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}

		RegisterPSO(desc.name.c_str(), pso);
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::DebugUVs);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = defaultMeshVSPath;
		desc.vsShader = defaultMeshVS;
		desc.psShader = debugUVsPS;

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::DebugVertexColor);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = defaultMeshVSPath;
		desc.vsShader = defaultMeshVS;
		desc.psShader = debugVertexColorPS;

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::DebugVertexNormals);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = defaultMeshVSPath;
		desc.vsShader = defaultMeshVS;
		desc.psShader = debugVertexNormalsPS;

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}
	}

	{
		PSODescription desc;
		desc.name = PSOName(PSOType::DebugTextureNormals);
		desc.inputLayoutDefinition = Vertex::InputLayoutDefinition;
		desc.vertexStride = sizeof(Vertex);
		desc.vsPath = defaultMeshVSPath;
		desc.vsShader = defaultMeshVS;
		desc.psShader = debugTextureNormalsPS;
		desc.samplerList[0] = "LinearWrapSS";

		std::shared_ptr<PipelineStateObject> pso = std::make_shared<PipelineStateObject>();
		if (!rv.CreatePSO(*pso, desc))
		{
			LOG(LogGraphicsEngine, Error, "Failed to create {} PSO", desc.name);
			return false;
		}
	}

	return true;
}

bool GraphicsEngine::CreateDefaultTextures(const std::filesystem::path& aContentRoot)
{
	myBRDFLUTTexture = std::make_shared<Texture>();
	myRHI->CreateLUT("LUT", 512, 512, myBRDFLUTTexture, aContentRoot / L"EngineAssets/Shaders/Quad_VS.cso", aContentRoot / L"EngineAssets/Shaders/brdfLUT_PS.cso");

	myDefaultAlbedoTexture = std::make_shared<Texture>();
	if (!GraphicsEngine::Get().GetResourceVendor().LoadTexture("default_c", BuiltIn_Default_C_ByteCode, sizeof(BuiltIn_Default_C_ByteCode), *myDefaultAlbedoTexture))
	{
		LOG(LogGraphicsEngine, Error, "Failed to register default albedo texture");
		return false;
	}

	myDefaultNormalTexture = std::make_shared<Texture>();
	if (!GraphicsEngine::Get().GetResourceVendor().LoadTexture("default_n", BuiltIn_Default_N_ByteCode, sizeof(BuiltIn_Default_N_ByteCode), *myDefaultNormalTexture))
	{
		LOG(LogGraphicsEngine, Error, "Failed to register default normal texture");
		return false;
	}

	myDefaultARMTexture = std::make_shared<Texture>();
	if (!GraphicsEngine::Get().GetResourceVendor().LoadTexture("default_m", BuiltIn_Default_M_ByteCode, sizeof(BuiltIn_Default_M_ByteCode), *myDefaultARMTexture))
	{
		LOG(LogGraphicsEngine, Error, "Failed to register default ARM texture");
		return false;
	}

	myDefaultFXTexture = std::make_shared<Texture>();
	if (!GraphicsEngine::Get().GetResourceVendor().LoadTexture("default_fx", BuiltIn_Default_FX_ByteCode, sizeof(BuiltIn_Default_FX_ByteCode), *myDefaultFXTexture))
	{
		LOG(LogGraphicsEngine, Error, "Failed to register default effects texture");
		return false;
	}

	return true;
}

bool GraphicsEngine::CreateConstantBuffers()
{
	ConstantBuffer frameBuffer;
	if (!myRHI->CreateConstantBuffer("FrameBuffer", sizeof(FrameBuffer), 0, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_GEOMETRY_SHADER | PIPELINE_STAGE_PIXEL_SHADER, frameBuffer))
	{
		LOG(LogGraphicsEngine, Error, "Failed to create Frame Buffer!");
		return false;
	}
	myConstantBuffers.emplace(ConstantBufferType::FrameBuffer, std::move(frameBuffer));

	ConstantBuffer objectBuffer;
	if (!myRHI->CreateConstantBuffer("ObjectBuffer", sizeof(ObjectBuffer), 1, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_GEOMETRY_SHADER | PIPELINE_STAGE_PIXEL_SHADER, objectBuffer))
	{
		LOG(LogGraphicsEngine, Error, "Failed to create Object Buffer!");
		return false;
	}
	myConstantBuffers.emplace(ConstantBufferType::ObjectBuffer, std::move(objectBuffer));

	ConstantBuffer animationBuffer;
	if (!myRHI->CreateConstantBuffer("AnimationBuffer", sizeof(AnimationBuffer), 2, PIPELINE_STAGE_VERTEX_SHADER, animationBuffer))
	{
		LOG(LogGraphicsEngine, Error, "Failed to create Animation Buffer!");
		return false;
	}
	myConstantBuffers.emplace(ConstantBufferType::AnimationBuffer, std::move(animationBuffer));

	ConstantBuffer materialBuffer;
	if (!myRHI->CreateConstantBuffer("MaterialBuffer", sizeof(MaterialBuffer), 3, PIPELINE_STAGE_PIXEL_SHADER, materialBuffer))
	{
		LOG(LogGraphicsEngine, Error, "Failed to create Material Buffer!");
		return false;
	}
	myConstantBuffers.emplace(ConstantBufferType::MaterialBuffer, std::move(materialBuffer));

	ConstantBuffer lightBuffer;
	if (!myRHI->CreateConstantBuffer("LightBuffer", sizeof(LightBuffer), 4, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER, lightBuffer))
	{
		LOG(LogGraphicsEngine, Error, "Failed to create Light Buffer!");
		return false;
	}
	myConstantBuffers.emplace(ConstantBufferType::LightBuffer, std::move(lightBuffer));

	ConstantBuffer shadowBuffer;
	if (!myRHI->CreateConstantBuffer("ShadowBuffer", sizeof(ShadowBuffer), 5, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_GEOMETRY_SHADER | PIPELINE_STAGE_PIXEL_SHADER, shadowBuffer))
	{
		LOG(LogGraphicsEngine, Error, "Failed to create Shadow Buffer!");
		return false;
	}
	myConstantBuffers.emplace(ConstantBufferType::ShadowBuffer, std::move(shadowBuffer));

	ConstantBuffer spriteBuffer;
	if (!myRHI->CreateConstantBuffer("SpriteBuffer", sizeof(SpriteBuffer), 6, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_GEOMETRY_SHADER | PIPELINE_STAGE_PIXEL_SHADER, spriteBuffer))
	{
		LOG(LogGraphicsEngine, Error, "Failed to create Sprite Buffer!");
		return false;
	}
	myConstantBuffers.emplace(ConstantBufferType::SpriteBuffer, std::move(spriteBuffer));

	ConstantBuffer postProcessBuffer;
	if (!myRHI->CreateConstantBuffer("PostProcessBuffer", sizeof(PostProcessBuffer), 7, PIPELINE_STAGE_PIXEL_SHADER, postProcessBuffer))
	{
		LOG(LogGraphicsEngine, Error, "Failed to create Post Process Buffer!");
		return false;
	}
	myConstantBuffers.emplace(ConstantBufferType::PostProcessBuffer, std::move(postProcessBuffer));

	return true;
}
