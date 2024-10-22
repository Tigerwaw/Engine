#include "Enginepch.h"
#include "Renderer.h"

#include "GameEngine/Engine.h"
#include "GameEngine/DebugDrawer/DebugDrawer.h"
#include "AssetManager/AssetManager.h"
#include "GraphicsEngine/GraphicsEngine.h"
#include "GraphicsEngine/Objects/Sprite.h"
#include "GraphicsEngine/Objects/Text/Text.h"
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/Intersections/AABB3D.hpp"
#include "GameEngine/Intersections/Intersection3D.hpp"
namespace CU = CommonUtilities;

#include "GameEngine/ComponentSystem/Scene.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Model.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "GameEngine/ComponentSystem/Components/Graphics/InstancedModel.h"
#include "GameEngine/ComponentSystem/Components/Graphics/DebugModel.h"
#include "GameEngine/ComponentSystem/Components/Graphics/ParticleSystem.h"
#include "GameEngine/ComponentSystem/Components/Graphics/TrailSystem.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Camera.h"
#include "GameEngine/ComponentSystem/Components/Lights/AmbientLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/DirectionalLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/PointLight.h"
#include "GameEngine/ComponentSystem/Components/Lights/SpotLight.h"
#include "GameEngine/ComponentSystem/Components/Physics/Colliders/BoxCollider.h"
#include "GameEngine/ComponentSystem/Components/Physics/Colliders/SphereCollider.h"

Renderer::Renderer() = default;
Renderer::~Renderer() = default;

void Renderer::RenderScene(Scene& aScene)
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), "Renderer Add Render Commands");
	if (GraphicsEngine::Get().CurrentDebugMode != DebugMode::None)
	{
		RenderForward(aScene);
	}
	else
	{
		RenderDeferred(aScene);
	}

	DrawTestUI();
}

void Renderer::RenderForward(Scene& aScene)
{
	GraphicsEngine& gfx = GraphicsEngine::Get();
	GraphicsCommandList& gfxList = gfx.GetGraphicsCommandList();

	if (gfx.RecalculateShadowFrustum)
	{
		aScene.myDirectionalLight->GetComponent<DirectionalLight>()->RecalculateShadowFrustum(aScene.myMainCamera, myVisibleObjectsBB);
		myVisibleObjectsBB.InitWithCenterAndExtents(CU::Vector3f(), CU::Vector3f());
	}

	gfxList.Enqueue<UpdatePostProcessBuffer>();
	QueueDirectionalLightShadows(aScene);
	QueuePointLightShadows(aScene);
	QueueSpotLightShadows(aScene);
	QueueUpdateLightBuffer(aScene);

	// Final Render
	gfxList.Enqueue<UpdateFrameBuffer>(aScene.myMainCamera->GetComponent<Camera>());
	if (gfx.CurrentDebugMode != DebugMode::None)
	{
		std::shared_ptr<PipelineStateObject> pso = AssetManager::Get().GetAsset<PSOAsset>(gfx.DebugModeNames[static_cast<int>(gfx.CurrentDebugMode)])->pso;
		gfxList.Enqueue<SetDefaultRenderTarget>();
		QueueGameObjects(aScene, aScene.myMainCamera->GetComponent<Camera>(), false, pso);
	}
	else
	{
		gfxList.Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_PBR")->pso);
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::HDR), gfx.GetDepthBuffer(), true, false);
		gfxList.Enqueue<SetTextureResource>(126, aScene.myAmbientLight->GetComponent<AmbientLight>()->GetCubemap());
		QueueShadowmapTextureResources(aScene);
		QueueGameObjects(aScene, aScene.myMainCamera->GetComponent<Camera>());
	}

	for (int i = 100; i < 110; i++)
	{
		gfxList.Enqueue<ClearTextureResource>(i);
	}
	gfxList.Enqueue<ClearTextureResource>(126);

	if (GraphicsEngine::Get().DrawGizmos)
	{
		QueueDebugGizmos(aScene, aScene.myMainCamera->GetComponent<Camera>());
	}

	QueueDebugLines(aScene);
	Engine::GetInstance().GetDebugDrawer().DrawObjects();

	if (gfx.CurrentDebugMode == DebugMode::None)
	{
		// Tonemapping
		gfxList.Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>(gfx.TonemapperNames[static_cast<unsigned>(gfx.Tonemapper)])->pso);
		gfxList.Enqueue<SetRenderTarget>(gfx.GetBackBuffer(), gfx.GetDepthBuffer(), true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::HDR));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);
	}
}

void Renderer::RenderDeferred(Scene& aScene)
{
	GraphicsEngine& gfx = GraphicsEngine::Get();
	GraphicsCommandList& gfxList = gfx.GetGraphicsCommandList();

	if (gfx.RecalculateShadowFrustum)
	{
		aScene.myDirectionalLight->GetComponent<DirectionalLight>()->RecalculateShadowFrustum(aScene.myMainCamera, myVisibleObjectsBB);
		myVisibleObjectsBB.InitWithCenterAndExtents(CU::Vector3f(), CU::Vector3f());
	}

	gfxList.Enqueue<UpdatePostProcessBuffer>();
	QueueDirectionalLightShadows(aScene);
	QueuePointLightShadows(aScene);
	QueueSpotLightShadows(aScene);

	// Deferred Objects
	gfxList.Enqueue<BeginEvent>("Draw Deferred Objects");
	gfxList.Enqueue<SetGBufferAsRenderTarget>();
	gfxList.Enqueue<UpdateFrameBuffer>(aScene.myMainCamera->GetComponent<Camera>());
	RenderDeferredObjects(aScene);
	gfxList.Enqueue<EndEvent>();
	

	// Light passes
	QueueUpdateLightBuffer(aScene);
	QueueShadowmapTextureResources(aScene);

	if (gfx.SSAOEnabled)
	{
		// Downsampled SSAO
		gfxList.Enqueue<BeginEvent>("SSAO Pass");
		gfxList.Enqueue<SetTextureResource>(51, AssetManager::Get().GetAsset<TextureAsset>("T_BlueNoise")->texture);
		gfxList.Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_SSAO")->pso);
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::HalfScreenA), nullptr, true, false);
		gfxList.Enqueue<SetGBufferAsResource>();
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(51);
		gfxList.Enqueue<EndEvent>();

		// Blur
		gfxList.Enqueue<BeginEvent>("Blur Pass");
		gfxList.Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_RadialBlur")->pso);
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::Luminance), nullptr, true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::HalfScreenA));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);
		gfxList.Enqueue<EndEvent>();
	}

	// Directional Light
	gfxList.Enqueue<BeginEvent>("Ambient & Directional Light Pass");
	gfxList.Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_DeferredDirectionalLight")->pso);
	gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::HDR), nullptr, true, false);
	gfxList.Enqueue<SetTextureResource>(126, aScene.myAmbientLight->GetComponent<AmbientLight>()->GetCubemap());
	gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::Luminance));
	gfxList.Enqueue<SetGBufferAsResource>();
	gfxList.Enqueue<RenderFullscreenQuad>();
	gfxList.Enqueue<ClearTextureResource>(30);
	gfxList.Enqueue<ClearTextureResource>(126);
	gfxList.Enqueue<EndEvent>();

	// Pointlights
	gfxList.Enqueue<BeginEvent>("Pointlight Pass");
	gfxList.Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_DeferredPointlight")->pso);
	gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::HDR), nullptr, false, false);
	gfxList.Enqueue<SetGBufferAsResource>();
	gfxList.Enqueue<RenderFullscreenQuad>();
	gfxList.Enqueue<EndEvent>();

	// Spotlights
	gfxList.Enqueue<BeginEvent>("Spotlight Pass");
	gfxList.Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_DeferredSpotlight")->pso);
	gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::HDR), nullptr, false, false);
	gfxList.Enqueue<SetGBufferAsResource>();
	gfxList.Enqueue<RenderFullscreenQuad>();
	gfxList.Enqueue<EndEvent>();

	gfxList.Enqueue<BeginEvent>("Clear Texture Resources");
	for (int i = 0; i < 5; i++)
	{
		gfxList.Enqueue<ClearTextureResource>(i);
	}

	for (int i = 100; i < 110; i++)
	{
		gfxList.Enqueue<ClearTextureResource>(i);
	}
	gfxList.Enqueue<EndEvent>();
	
	// Forward meshes
	gfxList.Enqueue<BeginEvent>("Draw Forward Objects");
	gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::HDR), gfx.GetDepthBuffer(), false, false);
	RenderForwardObjects(aScene);
	gfxList.Enqueue<EndEvent>();

	if (GraphicsEngine::Get().DrawGizmos)
	{
		QueueDebugGizmos(aScene, aScene.myMainCamera->GetComponent<Camera>());
	}

	QueueDebugLines(aScene);
	Engine::GetInstance().GetDebugDrawer().DrawObjects();

	std::shared_ptr<Texture> renderTarget;
	if (gfx.BloomEnabled)
	{
		renderTarget = gfx.GetIntermediateTexture(IntermediateTexture::LDR);
	}
	else
	{
		renderTarget = gfx.GetBackBuffer();
	}

	// Tonemapping
	gfxList.Enqueue<BeginEvent>("Tonemapping Pass");
	gfxList.Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>(gfx.TonemapperNames[static_cast<unsigned>(gfx.Tonemapper)])->pso);
	gfxList.Enqueue<SetRenderTarget>(renderTarget, nullptr, true, false);
	gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::HDR));
	gfxList.Enqueue<RenderFullscreenQuad>();
	gfxList.Enqueue<ClearTextureResource>(30);
	gfxList.Enqueue<EndEvent>();

	// Particle & Trail Systems
	gfxList.Enqueue<BeginEvent>("Render Particle Systems");
	gfxList.Enqueue<SetRenderTarget>(renderTarget, gfx.GetDepthBuffer(), false, false);

	for (auto& gameObject : aScene.myGameObjects)
	{
		std::shared_ptr<ParticleSystem> particleSystem = gameObject->GetComponent<ParticleSystem>();
		if (particleSystem && particleSystem->GetActive())
		{
			GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderParticles>(particleSystem);
		}

		std::shared_ptr<TrailSystem> trailSystem = gameObject->GetComponent<TrailSystem>();
		if (trailSystem && trailSystem->GetActive())
		{
			GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderTrail>(trailSystem);
		}
	}
	gfxList.Enqueue<EndEvent>();

	if (gfx.BloomEnabled)
	{
		// Luminance
		gfxList.Enqueue<BeginEvent>("Luminance Pass");
		gfxList.Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_Luminance")->pso);
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::Luminance), nullptr, true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::LDR));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);
		gfxList.Enqueue<EndEvent>();

		// Downsample
		gfxList.Enqueue<BeginEvent>("Downsampling Pass");
		gfxList.Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_Resample")->pso);
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::HalfScreenA), nullptr, true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::Luminance));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);

		gfxList.Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_Resample")->pso);
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenA), nullptr, true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::HalfScreenA));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);
		gfxList.Enqueue<EndEvent>();

		// Blur
		gfxList.Enqueue<BeginEvent>("Blur Pass");
		gfxList.Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_GaussianH")->pso);
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenB), nullptr, true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenA));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);

		gfxList.Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_GaussianV")->pso);
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenA), nullptr, true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenB));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);

		gfxList.Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_GaussianH")->pso);
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenB), nullptr, false, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenA));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);

		gfxList.Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_GaussianV")->pso);
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenA), nullptr, false, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenB));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);
		gfxList.Enqueue<EndEvent>();

		// Upsample
		gfxList.Enqueue<BeginEvent>("Upsampling Pass");
		gfxList.Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_Resample")->pso);
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::HalfScreenA), nullptr, true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::QuarterScreenA));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);

		gfxList.Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_Resample")->pso);
		gfxList.Enqueue<SetRenderTarget>(gfx.GetIntermediateTexture(IntermediateTexture::Luminance), nullptr, true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::HalfScreenA));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);
		gfxList.Enqueue<EndEvent>();

		// Bloom
		gfxList.Enqueue<BeginEvent>("Bloom Pass");
		gfxList.Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_Bloom")->pso);
		gfxList.Enqueue<SetRenderTarget>(gfx.GetBackBuffer(), nullptr, true, false);
		gfxList.Enqueue<SetTextureResource>(30, gfx.GetIntermediateTexture(IntermediateTexture::Luminance));
		gfxList.Enqueue<SetTextureResource>(31, gfx.GetIntermediateTexture(IntermediateTexture::LDR));
		gfxList.Enqueue<RenderFullscreenQuad>();
		gfxList.Enqueue<ClearTextureResource>(30);
		gfxList.Enqueue<ClearTextureResource>(31);
		gfxList.Enqueue<EndEvent>();
	}
}

void Renderer::RenderDeferredObjects(Scene& aScene, bool aDisableViewCulling)
{
	std::shared_ptr<Camera> renderCamera = aScene.myMainCamera->GetComponent<Camera>();

	for (auto& gameObject : aScene.myGameObjects)
	{
		if (!gameObject->GetActive()) continue;

		std::shared_ptr<Model> model = gameObject->GetComponent<Model>();
		if (model && model->GetActive())
		{
			if (aDisableViewCulling || !model->GetShouldViewcull() || IsInsideFrustum(renderCamera, gameObject->GetComponent<Transform>(), model->GetBoundingBox()))
			{
				if (model->GetMaterialOnSlot(0)->GetPSO()->BlendState == nullptr)
				{
					UpdateBoundingBox(gameObject);
					GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMesh>(model, AssetManager::Get().GetAsset<PSOAsset>("PSO_Deferred")->pso);
				}
			}
		}

		std::shared_ptr<AnimatedModel> animModel = gameObject->GetComponent<AnimatedModel>();
		if (animModel && animModel->GetActive())
		{
			if (aDisableViewCulling || !animModel->GetShouldViewcull() || IsInsideFrustum(renderCamera, gameObject->GetComponent<Transform>(), animModel->GetBoundingBox()))
			{
				if (animModel->GetMaterialOnSlot(0)->GetPSO()->BlendState == nullptr)
				{
					UpdateBoundingBox(gameObject);
					GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMesh>(animModel, AssetManager::Get().GetAsset<PSOAsset>("PSO_Deferred")->pso);
				}
			}
		}

		std::shared_ptr<InstancedModel> instancedModel = gameObject->GetComponent<InstancedModel>();
		if (instancedModel && instancedModel->GetActive())
		{
			if (aDisableViewCulling || !instancedModel->GetShouldViewcull() || IsInsideFrustum(renderCamera, gameObject->GetComponent<Transform>(), instancedModel->GetBoundingBox()))
			{
				if (instancedModel->GetMaterialOnSlot(0)->GetPSO()->BlendState == nullptr)
				{
					UpdateBoundingBox(gameObject);
					GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderInstancedMesh>(instancedModel, AssetManager::Get().GetAsset<PSOAsset>("PSO_Deferred")->pso);
				}
			}
		}
	}
}

void Renderer::RenderForwardObjects(Scene& aScene, bool aDisableViewCulling)
{
	std::shared_ptr<Camera> renderCamera = aScene.myMainCamera->GetComponent<Camera>();

	for (auto& gameObject : aScene.myGameObjects)
	{
		if (!gameObject->GetActive()) continue;

		std::shared_ptr<Model> model = gameObject->GetComponent<Model>();
		if (model && model->GetActive())
		{
			if (aDisableViewCulling || !model->GetShouldViewcull() || IsInsideFrustum(renderCamera, gameObject->GetComponent<Transform>(), model->GetBoundingBox()))
			{
				if (model->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr)
				{
					GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMesh>(model);
				}
			}
		}

		std::shared_ptr<AnimatedModel> animModel = gameObject->GetComponent<AnimatedModel>();
		if (animModel && animModel->GetActive())
		{
			if (aDisableViewCulling || !animModel->GetShouldViewcull() || IsInsideFrustum(renderCamera, gameObject->GetComponent<Transform>(), animModel->GetBoundingBox()))
			{
				if (animModel->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr)
				{
					GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMesh>(animModel);
				}
			}
		}

		std::shared_ptr<InstancedModel> instancedModel = gameObject->GetComponent<InstancedModel>();
		if (instancedModel && instancedModel->GetActive())
		{
			if (aDisableViewCulling || !instancedModel->GetShouldViewcull() || IsInsideFrustum(renderCamera, gameObject->GetComponent<Transform>(), instancedModel->GetBoundingBox()))
			{
				if (instancedModel->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr)
				{
					GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderInstancedMesh>(instancedModel);
				}
			}
		}
	}
}

void Renderer::QueueShadowmapTextureResources(Scene& aScene)
{
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetMarker>("Set Shadowmaps");

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetTextureResource>(100, aScene.myDirectionalLight->GetComponent<DirectionalLight>()->GetShadowMap());

	for (int i = 0; i < aScene.myPointLights.size(); i++)
	{
		if (i >= MAX_POINTLIGHTS) break;

		std::shared_ptr<PointLight> pLight = aScene.myPointLights[i]->GetComponent<PointLight>();
		if (!pLight->GetActive()) continue;
		if (!pLight->CastsShadows()) continue;

		std::shared_ptr<Texture> shadowMap = pLight->GetShadowMap();
		if (!shadowMap) continue;

		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetTextureResource>(101 + i, shadowMap);
	}

	for (int i = 0; i < aScene.mySpotLights.size(); i++)
	{
		if (i >= MAX_SPOTLIGHTS) break;

		std::shared_ptr<SpotLight> sLight = aScene.mySpotLights[i]->GetComponent<SpotLight>();
		if (!sLight->GetActive()) continue;
		if (!sLight->CastsShadows()) continue;

		std::shared_ptr<Texture> shadowMap = sLight->GetShadowMap();
		if (!shadowMap) continue;

		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetTextureResource>(105 + i, shadowMap);
	}
}

void Renderer::QueueUpdateLightBuffer(Scene& aScene)
{
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetMarker>("Update Light Buffer");

	std::shared_ptr<AmbientLight> ambientLight;
	if (aScene.myAmbientLight)
	{
		ambientLight = aScene.myAmbientLight->GetComponent<AmbientLight>();
	}

	std::shared_ptr<DirectionalLight> dirLight;
	if (aScene.myDirectionalLight)
	{
		dirLight = aScene.myDirectionalLight->GetComponent<DirectionalLight>();
	}

	std::vector<std::shared_ptr<PointLight>> pointLights;
	for (auto& plight : aScene.myPointLights)
	{
		if (plight->GetActive())
		{
			pointLights.emplace_back(plight->GetComponent<PointLight>());
		}
	}
	std::vector<std::shared_ptr<SpotLight>> spotLights;
	for (auto& slight : aScene.mySpotLights)
	{
		if (slight->GetActive())
		{
			spotLights.emplace_back(slight->GetComponent<SpotLight>());
		}
	}

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateLightBuffer>(ambientLight, dirLight, pointLights, spotLights);
}

void Renderer::QueueSpotLightShadows(Scene& aScene)
{
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<BeginEvent>("Spotlight Shadows");

	for (int i = 0; i < aScene.mySpotLights.size(); i++)
	{
		std::shared_ptr<SpotLight> spotLight = aScene.mySpotLights[i]->GetComponent<SpotLight>();
		if (!spotLight->GetActive()) continue;
		if (!spotLight->CastsShadows()) continue;

		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_Shadow")->pso);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, spotLight->GetShadowMap(), false, true);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(aScene.mySpotLights[i]->GetComponent<Camera>());
		QueueGameObjects(aScene, aScene.mySpotLights[i]->GetComponent<Camera>(), false, AssetManager::Get().GetAsset<PSOAsset>("PSO_Shadow")->pso);
	}

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<EndEvent>();
}

void Renderer::QueuePointLightShadows(Scene& aScene)
{
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<BeginEvent>("Pointlight Shadows");

	for (int i = 0; i < aScene.myPointLights.size(); i++)
	{
		std::shared_ptr<PointLight> pointLight = aScene.myPointLights[i]->GetComponent<PointLight>();
		if (!pointLight->GetActive()) continue;
		if (!pointLight->CastsShadows()) continue;

		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, pointLight->GetShadowMap(), false, true);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_ShadowCube")->pso);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(aScene.myPointLights[i]->GetComponent<Camera>());
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateShadowBuffer>(pointLight);
		QueueGameObjects(aScene, pointLight, false, AssetManager::Get().GetAsset<PSOAsset>("PSO_ShadowCube")->pso);
	}

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<EndEvent>();
}

void Renderer::QueueDirectionalLightShadows(Scene& aScene)
{
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<BeginEvent>("Directional Light Shadows");

	std::shared_ptr<DirectionalLight> dLight = aScene.myDirectionalLight->GetComponent<DirectionalLight>();
	if (!dLight->GetActive()) return;
	if (!dLight->CastsShadows()) return;

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, dLight->GetShadowMap(), false, true);
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_Shadow")->pso);
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(aScene.myDirectionalLight->GetComponent<Camera>());
	QueueGameObjects(aScene, aScene.myDirectionalLight->GetComponent<Camera>(), false, AssetManager::Get().GetAsset<PSOAsset>("PSO_Shadow")->pso);
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<EndEvent>();
}

void Renderer::QueueDebugGizmos(Scene& aScene, std::shared_ptr<Camera> aRenderCamera)
{
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<BeginEvent>("Draw Debug Gizmos");

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_Gizmo")->pso);
	CU::PlaneVolume<float> frustumVolume = aRenderCamera->GetFrustumPlaneVolume();

	for (auto& gameObject : aScene.myGameObjects)
	{
		std::shared_ptr<DebugModel> model = gameObject->GetComponent<DebugModel>();
		if (model && model->GetActive())
		{
			if (aRenderCamera->GetViewcullingIntersection(gameObject->GetComponent<Transform>(), model->GetBoundingBox()))
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderDebugMesh>(model);
			}
		}
	}

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<EndEvent>();
}

void Renderer::QueueGameObjects(Scene& aScene, std::shared_ptr<Camera> aRenderCamera, bool aDisableViewCulling, std::shared_ptr<PipelineStateObject> aPSOoverride)
{
	for (auto& gameObject : aScene.myGameObjects)
	{
		if (!gameObject->GetActive()) continue;

		std::shared_ptr<Model> model = gameObject->GetComponent<Model>();
		if (model && model->GetActive())
		{
			if (aDisableViewCulling || !model->GetShouldViewcull() || IsInsideFrustum(aRenderCamera, gameObject->GetComponent<Transform>(), model->GetBoundingBox()))
			{
				if (aRenderCamera->gameObject->GetName() == "MainCamera")
				{
					UpdateBoundingBox(gameObject);
				}
				else
				{
					if (!model->GetCastShadows()) continue;
				}
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMesh>(model, aPSOoverride);
			}
		}

		std::shared_ptr<AnimatedModel> animModel = gameObject->GetComponent<AnimatedModel>();
		if (animModel && animModel->GetActive())
		{
			if (aDisableViewCulling || !animModel->GetShouldViewcull() || IsInsideFrustum(aRenderCamera, gameObject->GetComponent<Transform>(), animModel->GetBoundingBox()))
			{
				if (aRenderCamera->gameObject->GetName() == "MainCamera")
				{
					UpdateBoundingBox(gameObject);
				}
				else
				{
					if (!animModel->GetCastShadows()) continue;
				}

				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMesh>(animModel, aPSOoverride);
			}
		}

		std::shared_ptr<InstancedModel> instancedModel = gameObject->GetComponent<InstancedModel>();
		if (instancedModel && instancedModel->GetActive())
		{
			if (aDisableViewCulling || !instancedModel->GetShouldViewcull() || IsInsideFrustum(aRenderCamera, gameObject->GetComponent<Transform>(), instancedModel->GetBoundingBox()))
			{
				if (aRenderCamera->gameObject->GetName() == "MainCamera")
				{
					UpdateBoundingBox(gameObject);
				}
				else
				{
					if (!instancedModel->GetCastShadows()) continue;
				}

				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderInstancedMesh>(instancedModel, aPSOoverride);
			}
		}
	}
}

void Renderer::QueueGameObjects(Scene& aScene, std::shared_ptr<PointLight> aPointLight, bool aDisableViewCulling, std::shared_ptr<PipelineStateObject> aPSOoverride)
{
	for (auto& gameObject : aScene.myGameObjects)
	{
		if (!gameObject->GetActive()) continue;

		std::shared_ptr<Model> model = gameObject->GetComponent<Model>();
		if (model && model->GetActive())
		{
			if (!model->GetCastShadows()) continue;

			if (aDisableViewCulling || !model->GetShouldViewcull() || IsInsideRadius(aPointLight, gameObject->GetComponent<Transform>(), model->GetBoundingBox()))
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMesh>(model, aPSOoverride);
			}
		}

		std::shared_ptr<AnimatedModel> animModel = gameObject->GetComponent<AnimatedModel>();
		if (animModel && animModel->GetActive())
		{
			if (!animModel->GetCastShadows()) continue;

			if (aDisableViewCulling || !animModel->GetShouldViewcull() || IsInsideRadius(aPointLight, gameObject->GetComponent<Transform>(), animModel->GetBoundingBox()))
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMesh>(animModel, aPSOoverride);
			}
		}

		std::shared_ptr<InstancedModel> instancedModel = gameObject->GetComponent<InstancedModel>();
		if (instancedModel && instancedModel->GetActive())
		{
			if (!instancedModel->GetCastShadows()) continue;

			if (aDisableViewCulling || !instancedModel->GetShouldViewcull() || IsInsideRadius(aPointLight, gameObject->GetComponent<Transform>(), instancedModel->GetBoundingBox()))
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderInstancedMesh>(instancedModel, aPSOoverride);
			}
		}
	}
}

void Renderer::QueueDebugLines(Scene& aScene)
{
	if (GraphicsEngine::Get().DrawBoundingBoxes)
	{
		Engine::GetInstance().GetDebugDrawer().DrawBoundingBox(aScene.myBoundingBox);
		Engine::GetInstance().GetDebugDrawer().DrawBoundingBox(myVisibleObjectsBB);
	}

	for (auto& gameObject : aScene.myGameObjects)
	{
		if (!gameObject->GetActive()) continue;

		if (GraphicsEngine::Get().DrawCameraFrustums)
		{
			std::shared_ptr<Camera> cam = gameObject->GetComponent<Camera>();
			if (cam && cam->GetActive() && gameObject != aScene.myMainCamera)
			{
				Engine::GetInstance().GetDebugDrawer().DrawCameraFrustum(cam);
			}
		}

		if (GraphicsEngine::Get().DrawColliders)
		{
			std::shared_ptr<BoxCollider> boxCollider = gameObject->GetComponent<BoxCollider>();
			if (boxCollider && boxCollider->GetActive())
			{
				CU::Vector4f colliderColor = boxCollider->debugColliding ? CU::Vector4f(1.0f, 0, 0, 1.0f) : CU::Vector4f(0, 1.0f, 0, 1.0f);
				Engine::GetInstance().GetDebugDrawer().DrawBoundingBox(boxCollider->GetAABB(), gameObject->GetComponent<Transform>()->GetWorldMatrix(), colliderColor);
			}

			std::shared_ptr<SphereCollider> sphereCollider = gameObject->GetComponent<SphereCollider>();
			if (sphereCollider && sphereCollider->GetActive())
			{
				CU::Vector4f colliderColor = sphereCollider->debugColliding ? CU::Vector4f(1.0f, 0, 0, 1.0f) : CU::Vector4f(0, 1.0f, 0, 1.0f);
				Engine::GetInstance().GetDebugDrawer().DrawBoundingSphere(sphereCollider->GetSphere(), gameObject->GetComponent<Transform>()->GetWorldMatrix(), colliderColor);
			}
		}
	}
}

bool Renderer::IsInsideFrustum(std::shared_ptr<Camera> aRenderCamera, std::shared_ptr<Transform> aObjectTransform, CU::AABB3D<float> aObjectAABB)
{
	if (!GraphicsEngine::Get().UseViewCulling) return true;

	return aRenderCamera->GetViewcullingIntersection(aObjectTransform, aObjectAABB);
}

bool Renderer::IsInsideRadius(std::shared_ptr<PointLight> aPointLight, std::shared_ptr<Transform> aObjectTransform, CU::AABB3D<float> aObjectAABB)
{
	if (!GraphicsEngine::Get().UseViewCulling) return true;

	std::shared_ptr<Transform> pointLightTransform = aPointLight->gameObject->GetComponent<Transform>();
	std::shared_ptr<Camera> pointLightCam = aPointLight->gameObject->GetComponent<Camera>();
	if (!pointLightTransform) return true;

	CU::Matrix4x4f objectMatrix = aObjectTransform->GetWorldMatrix();

	if (aObjectTransform->IsScaled())
	{
		objectMatrix = objectMatrix.GetInverse();
	}
	else
	{
		objectMatrix = objectMatrix.GetFastInverse();
	}

	CU::Sphere<float> sphere(pointLightTransform->GetTranslation(), pointLightCam->GetFarPlane());
	sphere = sphere.GetSphereinNewSpace(pointLightTransform->GetWorldMatrix() * objectMatrix);
	return CU::IntersectionSphereAABB(sphere, aObjectAABB);
}

void Renderer::UpdateBoundingBox(std::shared_ptr<GameObject> aGameObject)
{
	if (aGameObject->GetComponent<Transform>())
	{
		CU::Vector3f bbMin = myVisibleObjectsBB.GetMin();
		CU::Vector3f bbMax = myVisibleObjectsBB.GetMax();
		std::shared_ptr<Transform> objectTransform = aGameObject->GetComponent<Transform>();

		std::shared_ptr<Model> model = aGameObject->GetComponent<Model>();
		std::shared_ptr<AnimatedModel> animModel = aGameObject->GetComponent<AnimatedModel>();
		std::shared_ptr<InstancedModel> instancedModel = aGameObject->GetComponent<InstancedModel>();
		if (model)
		{
			if (!model->GetShouldViewcull()) return;

			auto& corners = model->GetBoundingBox().GetCorners();

			for (CU::Vector3f corner : corners)
			{
				corner = CU::ToVector3(CU::ToVector4(corner, 1.0f) * objectTransform->GetWorldMatrix());

				bbMin.x = std::fminf(corner.x, bbMin.x);
				bbMax.x = std::fmaxf(corner.x, bbMax.x);
				bbMin.y = std::fminf(corner.y, bbMin.y);
				bbMax.y = std::fmaxf(corner.y, bbMax.y);
				bbMin.z = std::fminf(corner.z, bbMin.z);
				bbMax.z = std::fmaxf(corner.z, bbMax.z);
			}
		}
		else if (animModel)
		{
			if (!animModel->GetShouldViewcull()) return;

			auto& corners = animModel->GetBoundingBox().GetCorners();
			for (CU::Vector3f corner : corners)
			{
				corner = CU::ToVector3(CU::ToVector4(corner, 1.0f) * objectTransform->GetWorldMatrix());

				bbMin.x = std::fminf(corner.x, bbMin.x);
				bbMax.x = std::fmaxf(corner.x, bbMax.x);
				bbMin.y = std::fminf(corner.y, bbMin.y);
				bbMax.y = std::fmaxf(corner.y, bbMax.y);
				bbMin.z = std::fminf(corner.z, bbMin.z);
				bbMax.z = std::fmaxf(corner.z, bbMax.z);
			}
		}
		else if (instancedModel)
		{
			if (!instancedModel->GetShouldViewcull()) return;

			auto& corners = instancedModel->GetBoundingBox().GetCorners();
			for (CU::Vector3f corner : corners)
			{
				corner = CU::ToVector3(CU::ToVector4(corner, 1.0f) * objectTransform->GetWorldMatrix());

				bbMin.x = std::fminf(corner.x, bbMin.x);
				bbMax.x = std::fmaxf(corner.x, bbMax.x);
				bbMin.y = std::fminf(corner.y, bbMin.y);
				bbMax.y = std::fmaxf(corner.y, bbMax.y);
				bbMin.z = std::fminf(corner.z, bbMin.z);
				bbMax.z = std::fmaxf(corner.z, bbMax.z);
			}
		}
		else
		{
			CU::Vector3f point = objectTransform->GetTranslation(true);

			bbMin.x = std::fminf(point.x, bbMin.x);
			bbMax.x = std::fmaxf(point.x, bbMax.x);
			bbMin.y = std::fminf(point.y, bbMin.y);
			bbMax.y = std::fmaxf(point.y, bbMax.y);
			bbMin.z = std::fminf(point.z, bbMin.z);
			bbMax.z = std::fmaxf(point.z, bbMax.z);
		}

		myVisibleObjectsBB.InitWithMinAndMax(bbMin, bbMax);
	}
}

// TEMP
void Renderer::Init()
{
	//myTestSprite = std::make_shared<Sprite>();
	//myTestText = std::make_shared<Text>();

	//myTestText->SetFont(AssetManager::Get().GetAsset<FontAsset>("Fonts/F_RobotoRegular.json")->font);
	//myTestText->SetPosition(CU::Vector2f(-500.0f, 700.0f));
	//myTestText->SetSize(5);
	//myTestText->SetTextContent("Test");
}

void Renderer::DrawTestUI()
{
	//myTestSprite->SetTexture(AssetManager::Get().GetAsset<TextureAsset>("EngineAssets/Textures/Utility/T_perlin_C.dds")->texture);
	//myTestSprite->SetPosition(CU::Vector2f(500.0f, 500.0f));
	//myTestSprite->SetSize(CU::Vector2f(600.0f, 600.0f));
	//GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(GraphicsEngine::Get().GetBackBuffer(), nullptr, false, false);
	//GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderSprite>(myTestSprite);

	//GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderText>(myTestText);
}