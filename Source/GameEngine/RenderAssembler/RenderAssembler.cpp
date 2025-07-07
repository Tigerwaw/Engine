#include "Enginepch.h"
#include "RenderAssembler.h"

#include "Engine.h"
#include "DebugDrawer/DebugDrawer.h"
#include "AssetManager.h"
#include "GraphicsEngine.h"
#include "Objects/Sprite.h"
#include "Objects/Text/Text.h"
#include "Math/Vector.hpp"
#include "Math/AABB3D.hpp"
#include "Math/Intersection3D.hpp"
#include "Time/Timer.h"

#include "ComponentSystem/Scene.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "ComponentSystem/Components/Graphics/Model.h"
#include "ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "ComponentSystem/Components/Graphics/InstancedModel.h"
#include "ComponentSystem/Components/Graphics/ParticleSystem.h"
#include "ComponentSystem/Components/Graphics/TrailSystem.h"
#include "ComponentSystem/Components/Graphics/Camera.h"
#include "ComponentSystem/Components/Lights/AmbientLight.h"
#include "ComponentSystem/Components/Lights/DirectionalLight.h"
#include "ComponentSystem/Components/Lights/PointLight.h"
#include "ComponentSystem/Components/Lights/SpotLight.h"
#include "ComponentSystem/Components/Physics/Colliders/BoxCollider.h"
#include "ComponentSystem/Components/Physics/Colliders/SphereCollider.h"

RenderAssembler::RenderAssembler() = default;
RenderAssembler::~RenderAssembler() = default;

void RenderAssembler::RenderScene(Scene& aScene)
{
	PIXScopedEvent(PIX_COLOR_INDEX(6), "Renderer Add Render Commands");

	SceneRenderData sceneRenderData = AssembleLists(aScene);

	if (GraphicsEngine::Get().CurrentDebugMode != DebugMode::None)
	{
		RenderDebug(sceneRenderData);
	}
	else
	{
		RenderDeferred(sceneRenderData);
	}

	DrawTestUI();
}

RenderAssembler::SceneRenderData RenderAssembler::AssembleLists(Scene& aScene)
{
	SceneRenderData sceneRenderData;
	sceneRenderData.castShadowsObjects.reserve(100);
	sceneRenderData.drawDeferred.reserve(100);
	sceneRenderData.drawForward.reserve(100);
	sceneRenderData.pointLights.reserve(4);
	sceneRenderData.spotLights.reserve(4);
	if (GraphicsEngine::Get().DrawColliders)
		sceneRenderData.drawCollidersObjects.reserve(100);
	if (GraphicsEngine::Get().DrawBoundingBoxes)
		sceneRenderData.drawBoundingBoxesObjects.reserve(100);

	sceneRenderData.sceneBoundingBox = aScene.myBoundingBox;

	for (auto& gameObject : aScene.myGameObjects)
	{
		if (!gameObject->GetActive()) continue;

		{
			std::shared_ptr<Model> model = gameObject->GetComponent<Model>();
			if (model && model->GetActive())
			{
				if (model->GetCastShadows())
				{
					sceneRenderData.castShadowsObjects.emplace_back(gameObject);
				}

				if (model->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr)
				{
					sceneRenderData.drawForward.emplace_back(gameObject);
				}
				else
				{
					sceneRenderData.drawDeferred.emplace_back(gameObject);
				}

				if (GraphicsEngine::Get().DrawBoundingBoxes)
				{
					sceneRenderData.drawBoundingBoxesObjects.emplace_back(gameObject);
				}
			}
		}

		{
			std::shared_ptr<AnimatedModel> animModel = gameObject->GetComponent<AnimatedModel>();
			if (animModel && animModel->GetActive())
			{
				if (animModel->GetCastShadows())
				{
					sceneRenderData.castShadowsObjects.emplace_back(gameObject);
				}

				if (animModel->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr)
				{
					sceneRenderData.drawForward.emplace_back(gameObject);
				}
				else
				{
					sceneRenderData.drawDeferred.emplace_back(gameObject);
				}

				if (GraphicsEngine::Get().DrawBoundingBoxes)
				{
					sceneRenderData.drawBoundingBoxesObjects.emplace_back(gameObject);
				}
			}
		}

		{
			std::shared_ptr<InstancedModel> instancedModel = gameObject->GetComponent<InstancedModel>();
			if (instancedModel && instancedModel->GetActive())
			{
				if (instancedModel->GetCastShadows())
				{
					sceneRenderData.castShadowsObjects.emplace_back(gameObject);
				}

				if (instancedModel->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr)
				{
					sceneRenderData.drawForward.emplace_back(gameObject);
				}
				else
				{
					sceneRenderData.drawDeferred.emplace_back(gameObject);
				}

				if (GraphicsEngine::Get().DrawBoundingBoxes)
				{
					sceneRenderData.drawBoundingBoxesObjects.emplace_back(gameObject);
				}
			}
		}

		{
			std::shared_ptr<ParticleSystem> particleSystem = gameObject->GetComponent<ParticleSystem>();
			if (particleSystem && particleSystem->GetActive())
			{
				sceneRenderData.particleSystems.emplace_back(particleSystem);
			}

			std::shared_ptr<TrailSystem> trailSystem = gameObject->GetComponent<TrailSystem>();
			if (trailSystem && trailSystem->GetActive())
			{
				sceneRenderData.trailSystems.emplace_back(trailSystem);
			}
		}

		{
			std::shared_ptr<AmbientLight> ambientLight = gameObject->GetComponent<AmbientLight>();
			if (ambientLight && ambientLight->GetActive())
			{
				sceneRenderData.ambientLight = ambientLight;
			}
		}

		{
			std::shared_ptr<DirectionalLight> directionalLight = gameObject->GetComponent<DirectionalLight>();
			if (directionalLight && directionalLight->GetActive())
			{
				sceneRenderData.directionalLight = directionalLight;
			}
		}

		{
			std::shared_ptr<PointLight> pointLight = gameObject->GetComponent<PointLight>();
			if (pointLight && pointLight->GetActive())
			{
				sceneRenderData.pointLights.emplace_back(pointLight);
			}
		}

		{
			std::shared_ptr<SpotLight> spotLight = gameObject->GetComponent<SpotLight>();
			if (spotLight && spotLight->GetActive())
			{
				sceneRenderData.spotLights.emplace_back(spotLight);
			}
		}

		std::shared_ptr<Camera> cam = gameObject->GetComponent<Camera>();
		if (cam && cam->GetActive())
		{
			if (cam->IsMainCamera())
			{
				sceneRenderData.mainCamera = cam;
			}
			else if (GraphicsEngine::Get().DrawCameraFrustums)
			{
				sceneRenderData.drawCameraFrustumsObjects.emplace_back(gameObject);
			}
		}

		if (GraphicsEngine::Get().DrawColliders)
		{
			std::shared_ptr<BoxCollider> boxCollider = gameObject->GetComponent<BoxCollider>();
			if (boxCollider && boxCollider->GetActive())
			{
				sceneRenderData.drawCollidersObjects.emplace_back(gameObject);
			}

			std::shared_ptr<SphereCollider> sphereCollider = gameObject->GetComponent<SphereCollider>();
			if (sphereCollider && sphereCollider->GetActive())
			{
				sceneRenderData.drawCollidersObjects.emplace_back(gameObject);
			}
		}
	}

	return sceneRenderData;
}

void RenderAssembler::RenderDebug(SceneRenderData& aRenderData)
{
	GraphicsEngine& gfx = GraphicsEngine::Get();
	GraphicsCommandList& gfxList = gfx.GetGraphicsCommandList();

	if (gfx.RecalculateShadowFrustum)
	{
		aRenderData.directionalLight->RecalculateShadowFrustum(aRenderData.mainCamera, myVisibleObjectsBB);
		myVisibleObjectsBB.InitWithCenterAndExtents(Math::Vector3f(), Math::Vector3f());
	}

	gfxList.Enqueue<UpdatePostProcessBuffer>();
	QueueDirectionalLightShadows(aRenderData);
	QueuePointLightShadows(aRenderData);
	QueueSpotLightShadows(aRenderData);
	QueueUpdateLightBuffer(aRenderData);

	// Final Render
	auto camTransform = aRenderData.mainCamera->gameObject->GetComponent<Transform>();
	FrameBuffer frameBuffer;
	frameBuffer.InvView = camTransform->GetWorldMatrix().GetFastInverse();
	frameBuffer.Projection = aRenderData.mainCamera->GetProjectionMatrix();
	frameBuffer.ViewPosition = Math::ToVector4(camTransform->GetTranslation(true), 1.0f);
	frameBuffer.ViewDirection = Math::ToVector4(camTransform->GetForwardVector(true), 1.0f);
	frameBuffer.NearPlane = aRenderData.mainCamera->GetNearPlane();
	frameBuffer.FarPlane = aRenderData.mainCamera->GetFarPlane();
	frameBuffer.Time = { static_cast<float>(Engine::Get().GetTimer().GetTimeSinceProgramStart()), Engine::Get().GetTimer().GetDeltaTime() };
	frameBuffer.Resolution = Engine::Get().GetResolution();

	gfxList.Enqueue<UpdateFrameBuffer>(frameBuffer);
	std::shared_ptr<PipelineStateObject> pso = AssetManager::Get().GetAsset<PSOAsset>(gfx.DebugModeNames[static_cast<int>(gfx.CurrentDebugMode)])->pso;
	gfxList.Enqueue<SetDefaultRenderTarget>();
	QueueObjectDebug(aRenderData);

	for (int i = 100; i < 110; i++)
	{
		gfxList.Enqueue<ClearTextureResource>(i);
	}
	gfxList.Enqueue<ClearTextureResource>(126);

	QueueDebugLines(aRenderData);
	Engine::Get().GetDebugDrawer().DrawObjects();
}

void RenderAssembler::RenderDeferred(SceneRenderData& aRenderData)
{
	GraphicsEngine& gfx = GraphicsEngine::Get();
	GraphicsCommandList& gfxList = gfx.GetGraphicsCommandList();

	if (gfx.RecalculateShadowFrustum)
	{
		aRenderData.directionalLight->RecalculateShadowFrustum(aRenderData.mainCamera, myVisibleObjectsBB);
		myVisibleObjectsBB.InitWithCenterAndExtents(Math::Vector3f(), Math::Vector3f());
	}

	gfxList.Enqueue<UpdatePostProcessBuffer>();
	QueueDirectionalLightShadows(aRenderData);
	QueuePointLightShadows(aRenderData);
	QueueSpotLightShadows(aRenderData);

	// Deferred Objects
	gfxList.Enqueue<BeginEvent>("Draw Deferred Objects");
	gfxList.Enqueue<SetGBufferAsRenderTarget>();

	auto camTransform = aRenderData.mainCamera->gameObject->GetComponent<Transform>();
	FrameBuffer frameBuffer;
	frameBuffer.InvView = camTransform->GetWorldMatrix().GetFastInverse();
	frameBuffer.Projection = aRenderData.mainCamera->GetProjectionMatrix();
	frameBuffer.ViewPosition = Math::ToVector4(camTransform->GetTranslation(true), 1.0f);
	frameBuffer.ViewDirection = Math::ToVector4(camTransform->GetForwardVector(true), 1.0f);
	frameBuffer.NearPlane = aRenderData.mainCamera->GetNearPlane();
	frameBuffer.FarPlane = aRenderData.mainCamera->GetFarPlane();
	frameBuffer.Time = { static_cast<float>(Engine::Get().GetTimer().GetTimeSinceProgramStart()), Engine::Get().GetTimer().GetDeltaTime() };
	frameBuffer.Resolution = Engine::Get().GetResolution();
	gfxList.Enqueue<UpdateFrameBuffer>(frameBuffer);
	QueueDeferredObjects(aRenderData);
	gfxList.Enqueue<EndEvent>();
	

	// Light passes
	QueueUpdateLightBuffer(aRenderData);
	QueueShadowmapTextureResources(aRenderData);

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
	gfxList.Enqueue<SetTextureResource>(126, aRenderData.ambientLight->GetCubemap());
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
	QueueForwardObjects(aRenderData);
	gfxList.Enqueue<EndEvent>();

	QueueDebugLines(aRenderData);
	Engine::Get().GetDebugDrawer().DrawObjects();

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
	QueueParticleSystems(aRenderData);
	QueueTrailSystems(aRenderData);
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

void RenderAssembler::QueueDeferredObjects(SceneRenderData& aRenderData)
{
	for (auto& gameObject : aRenderData.drawDeferred)
	{
		if (!gameObject->GetActive()) continue;
		auto transform = gameObject->GetComponent<Transform>();

		std::shared_ptr<Model> model = gameObject->GetComponent<Model>();
		if (model && model->GetActive())
		{
			if (!model->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, model->GetBoundingBox()))
			{
				UpdateBoundingBox(gameObject);

				RenderMesh::RenderMeshData data;
				data.mesh = model->GetMesh();
				data.transform = transform->GetWorldMatrix();
				data.materialList = model->GetMaterials();
				data.customShaderParams_1 = model->GetCustomShaderData_1();
				data.customShaderParams_2 = model->GetCustomShaderData_2();
				data.psoOverride = AssetManager::Get().GetAsset<PSOAsset>("PSO_Deferred")->pso;

				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMesh>(data);
			}
		}

		std::shared_ptr<AnimatedModel> animModel = gameObject->GetComponent<AnimatedModel>();
		if (animModel && animModel->GetActive())
		{
			if (!animModel->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, animModel->GetBoundingBox()))
			{
				UpdateBoundingBox(gameObject);

				RenderAnimatedMesh::AnimMeshRenderData data;
				data.mesh = animModel->GetMesh();
				data.transform = transform->GetWorldMatrix();
				data.materialList = animModel->GetMaterials();
				data.jointTransforms = animModel->GetCurrentPose();
				data.psoOverride = AssetManager::Get().GetAsset<PSOAsset>("PSO_Deferred")->pso;

				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMesh>(data);
			}
		}

		std::shared_ptr<InstancedModel> instancedModel = gameObject->GetComponent<InstancedModel>();
		if (instancedModel && instancedModel->GetActive())
		{
			if (!instancedModel->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, instancedModel->GetBoundingBox()))
			{
				UpdateBoundingBox(gameObject);

				RenderInstancedMesh::InstancedMeshRenderData data;
				data.mesh = instancedModel->GetMesh();
				data.transform = transform->GetWorldMatrix();
				data.materialList = instancedModel->GetMaterials();
				data.instanceBuffer = &instancedModel->GetInstanceBuffer();
				data.meshCount = instancedModel->GetMeshCount();
				data.psoOverride = AssetManager::Get().GetAsset<PSOAsset>("PSO_Deferred")->pso;

				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderInstancedMesh>(data);
			}
		}
	}
}

void RenderAssembler::QueueForwardObjects(SceneRenderData& aRenderData)
{
	for (auto& gameObject : aRenderData.drawForward)
	{
		if (!gameObject->GetActive()) continue;
		auto transform = gameObject->GetComponent<Transform>();

		std::shared_ptr<Model> model = gameObject->GetComponent<Model>();
		if (model && model->GetActive())
		{
			if (!model->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, model->GetBoundingBox()))
			{
				if (model->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr)
				{
					RenderMesh::RenderMeshData data;
					data.mesh = model->GetMesh();
					data.transform = transform->GetWorldMatrix();
					data.materialList = model->GetMaterials();
					data.customShaderParams_1 = model->GetCustomShaderData_1();
					data.customShaderParams_2 = model->GetCustomShaderData_2();

					GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMesh>(data);
				}
			}
		}

		std::shared_ptr<AnimatedModel> animModel = gameObject->GetComponent<AnimatedModel>();
		if (animModel && animModel->GetActive())
		{
			if (!animModel->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, animModel->GetBoundingBox()))
			{
				if (animModel->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr)
				{
					RenderAnimatedMesh::AnimMeshRenderData data;
					data.mesh = animModel->GetMesh();
					data.transform = transform->GetWorldMatrix();
					data.materialList = animModel->GetMaterials();
					data.jointTransforms = animModel->GetCurrentPose();

					GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMesh>(data);
				}
			}
		}

		std::shared_ptr<InstancedModel> instancedModel = gameObject->GetComponent<InstancedModel>();
		if (instancedModel && instancedModel->GetActive())
		{
			if (!instancedModel->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, instancedModel->GetBoundingBox()))
			{
				if (instancedModel->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr)
				{
					RenderInstancedMesh::InstancedMeshRenderData data;
					data.mesh = instancedModel->GetMesh();
					data.transform = transform->GetWorldMatrix();
					data.materialList = instancedModel->GetMaterials();
					data.instanceBuffer = &instancedModel->GetInstanceBuffer();
					data.meshCount = instancedModel->GetMeshCount();

					GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderInstancedMesh>(data);
				}
			}
		}
	}
}

void RenderAssembler::QueueParticleSystems(SceneRenderData& aRenderData)
{
	for (auto& particleSystem : aRenderData.particleSystems)
	{
		RenderParticles::RenderParticlesData data;
		data.emitters = particleSystem->GetEmitters();
		data.transform = particleSystem->gameObject->GetComponent<Transform>()->GetWorldMatrix();
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderParticles>(data);
	}
}

void RenderAssembler::QueueTrailSystems(SceneRenderData& aRenderData)
{
	for (auto& trailSystem : aRenderData.trailSystems)
	{
		RenderTrail::TrailData data;
		data.emitters = trailSystem->GetEmitters();
		data.transform = trailSystem->gameObject->GetComponent<Transform>()->GetWorldMatrix();
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderTrail>(data);
	}
}

void RenderAssembler::QueueShadowmapTextureResources(SceneRenderData& aRenderData)
{
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetMarker>("Set Shadowmaps");

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetTextureResource>(100, aRenderData.directionalLight->GetShadowMap());

	for (int i = 0; i < aRenderData.pointLights.size(); i++)
	{
		if (i >= MAX_POINTLIGHTS) break;

		std::shared_ptr<PointLight> pLight = aRenderData.pointLights[i];
		if (!pLight->GetActive()) continue;
		if (!pLight->CastsShadows()) continue;

		std::shared_ptr<Texture> shadowMap = pLight->GetShadowMap();
		if (!shadowMap) continue;

		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetTextureResource>(101 + i, shadowMap);
	}

	for (int i = 0; i < aRenderData.spotLights.size(); i++)
	{
		if (i >= MAX_SPOTLIGHTS) break;

		std::shared_ptr<SpotLight> sLight = aRenderData.spotLights[i];
		if (!sLight->GetActive()) continue;
		if (!sLight->CastsShadows()) continue;

		std::shared_ptr<Texture> shadowMap = sLight->GetShadowMap();
		if (!shadowMap) continue;

		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetTextureResource>(105 + i, shadowMap);
	}
}

void RenderAssembler::QueueUpdateLightBuffer(SceneRenderData& aRenderData)
{
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetMarker>("Update Light Buffer");

	LightBuffer lightBufferData;

	if (aRenderData.ambientLight)
	{
		lightBufferData.AmbientLight.Color = aRenderData.ambientLight->GetColor();
		lightBufferData.AmbientLight.Intensity = aRenderData.ambientLight->GetIntensity();
	}

	if (aRenderData.directionalLight)
	{
		if (auto dirLightCamComp = aRenderData.directionalLight->gameObject->GetComponent<Camera>())
		{
			lightBufferData.DirLight.Projection = dirLightCamComp->GetProjectionMatrix();
			lightBufferData.DirLight.FrustumSize = dirLightCamComp->GetViewportDimensions();
			lightBufferData.DirLight.NearPlane = dirLightCamComp->GetNearPlane();
		}

		if (auto dirLightTransformComp = aRenderData.directionalLight->gameObject->GetComponent<Transform>())
		{
			lightBufferData.DirLight.View = dirLightTransformComp->GetWorldMatrix().GetFastInverse();
		}

		lightBufferData.DirLight.Color = aRenderData.directionalLight->GetColor();
		lightBufferData.DirLight.Intensity = aRenderData.directionalLight->GetIntensity();
		lightBufferData.DirLight.Direction = aRenderData.directionalLight->GetDirection();

		lightBufferData.DirLight.CastShadows = aRenderData.directionalLight->CastsShadows();
		lightBufferData.DirLight.MinBias = aRenderData.directionalLight->GetMinShadowBias();
		lightBufferData.DirLight.MaxBias = aRenderData.directionalLight->GetMaxShadowBias();
		lightBufferData.DirLight.LightSize = aRenderData.directionalLight->GetLightSize();
	}

	int activePLights = 0;
	for (int i = 0; i < static_cast<int>(aRenderData.pointLights.size()); i++)
	{
		auto& pLight = aRenderData.pointLights[i];

		if (pLight && pLight->GetActive())
		{
			++activePLights;

			if (auto cam = pLight->gameObject->GetComponent<Camera>())
			{
				lightBufferData.PointLights[i].FrustumSize = cam->GetViewportDimensions();
				lightBufferData.PointLights[i].NearPlane = cam->GetNearPlane();
				lightBufferData.PointLights[i].Projection = cam->GetProjectionMatrix();
			}

			lightBufferData.PointLights[i].Position = pLight->GetPosition();
			lightBufferData.PointLights[i].Color = pLight->GetColor();
			lightBufferData.PointLights[i].Intensity = pLight->GetIntensity();
			lightBufferData.PointLights[i].CastShadows = pLight->CastsShadows();
			lightBufferData.PointLights[i].MinBias = pLight->GetMinShadowBias();
			lightBufferData.PointLights[i].MaxBias = pLight->GetMaxShadowBias();
			lightBufferData.PointLights[i].LightSize = pLight->GetLightSize();
		}
	}
	lightBufferData.NumPointLights = activePLights;

	int activeSLights = 0;
	for (int i = 0; i < static_cast<int>(aRenderData.spotLights.size()); i++)
	{
		auto& sLight = aRenderData.spotLights[i];

		if (sLight && sLight->GetActive())
		{
			++activeSLights;

			if (auto transform = sLight->gameObject->GetComponent<Transform>())
			{
				lightBufferData.SpotLights[i].View = transform->GetMatrix().GetFastInverse();
			}

			if (auto cam = sLight->gameObject->GetComponent<Camera>())
			{
				lightBufferData.SpotLights[i].FrustumSize = cam->GetViewportDimensions();
				lightBufferData.SpotLights[i].NearPlane = cam->GetNearPlane();
				lightBufferData.SpotLights[i].Projection = cam->GetProjectionMatrix();
			}

			lightBufferData.SpotLights[i].Position = sLight->GetPosition();
			lightBufferData.SpotLights[i].Position = sLight->GetDirection();
			lightBufferData.SpotLights[i].ConeAngle = sLight->GetConeAngleRadians();
			lightBufferData.SpotLights[i].Color = sLight->GetColor();
			lightBufferData.SpotLights[i].Intensity = sLight->GetIntensity();
			lightBufferData.SpotLights[i].CastShadows = sLight->CastsShadows();
			lightBufferData.SpotLights[i].MinBias = sLight->GetMinShadowBias();
			lightBufferData.SpotLights[i].MaxBias = sLight->GetMaxShadowBias();
			lightBufferData.SpotLights[i].LightSize = sLight->GetLightSize();
		}
	}
	lightBufferData.NumSpotLights = activePLights;

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateLightBuffer>(lightBufferData);
}

void RenderAssembler::QueueSpotLightShadows(SceneRenderData& aRenderData)
{
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<BeginEvent>("Spotlight Shadows");

	for (int i = 0; i < aRenderData.spotLights.size(); i++)
	{
		std::shared_ptr<SpotLight> spotLight = aRenderData.spotLights[i];
		if (!spotLight->GetActive()) continue;
		if (!spotLight->CastsShadows()) continue;

		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_Shadow")->pso);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, spotLight->GetShadowMap(), false, true);

		std::shared_ptr<Camera> lightCam = spotLight->gameObject->GetComponent<Camera>();
		std::shared_ptr<Transform> lightTransform = spotLight->gameObject->GetComponent<Transform>();

		FrameBuffer frameBuffer;
		frameBuffer.InvView = lightTransform->GetWorldMatrix().GetFastInverse();
		frameBuffer.Projection = lightCam->GetProjectionMatrix();
		frameBuffer.ViewPosition = Math::ToVector4(lightTransform->GetTranslation(true), 1.0f);
		frameBuffer.ViewDirection = Math::ToVector4(lightTransform->GetForwardVector(true), 1.0f);
		frameBuffer.NearPlane = lightCam->GetNearPlane();
		frameBuffer.FarPlane = lightCam->GetFarPlane();
		frameBuffer.Time = { static_cast<float>(Engine::Get().GetTimer().GetTimeSinceProgramStart()), Engine::Get().GetTimer().GetDeltaTime() };
		frameBuffer.Resolution = Engine::Get().GetResolution();
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(frameBuffer);

		QueueObjectShadows(aRenderData, lightCam);
	}

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<EndEvent>();
}

void RenderAssembler::QueuePointLightShadows(SceneRenderData& aRenderData)
{
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<BeginEvent>("Pointlight Shadows");

	for (int i = 0; i < aRenderData.pointLights.size(); i++)
	{
		std::shared_ptr<PointLight> pointLight = aRenderData.pointLights[i];
		if (!pointLight->GetActive()) continue;
		if (!pointLight->CastsShadows()) continue;

		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, pointLight->GetShadowMap(), false, true);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_ShadowCube")->pso);

		std::shared_ptr<Camera> lightCam = pointLight->gameObject->GetComponent<Camera>();
		std::shared_ptr<Transform> lightTransform = pointLight->gameObject->GetComponent<Transform>();

		FrameBuffer frameBuffer;
		frameBuffer.InvView = lightTransform->GetWorldMatrix().GetFastInverse();
		frameBuffer.Projection = lightCam->GetProjectionMatrix();
		frameBuffer.ViewPosition = Math::ToVector4(lightTransform->GetTranslation(true), 1.0f);
		frameBuffer.ViewDirection = Math::ToVector4(lightTransform->GetForwardVector(true), 1.0f);
		frameBuffer.NearPlane = lightCam->GetNearPlane();
		frameBuffer.FarPlane = lightCam->GetFarPlane();
		frameBuffer.Time = { static_cast<float>(Engine::Get().GetTimer().GetTimeSinceProgramStart()), Engine::Get().GetTimer().GetDeltaTime() };
		frameBuffer.Resolution = Engine::Get().GetResolution();
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(frameBuffer);

		UpdateShadowBuffer::ShadowData shadowData;
		shadowData.cameraTransform = pointLight->gameObject->GetComponent<Transform>()->GetMatrix();
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateShadowBuffer>(shadowData);
		QueueObjectShadows(aRenderData, pointLight);
	}

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<EndEvent>();
}

void RenderAssembler::QueueDirectionalLightShadows(SceneRenderData& aRenderData)
{
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<BeginEvent>("Directional Light Shadows");

	std::shared_ptr<DirectionalLight> dLight = aRenderData.directionalLight;
	if (!dLight->GetActive()) return;
	if (!dLight->CastsShadows()) return;

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, dLight->GetShadowMap(), false, true);
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_Shadow")->pso);

	std::shared_ptr<Camera> lightCam = dLight->gameObject->GetComponent<Camera>();
	std::shared_ptr<Transform> lightTransform = dLight->gameObject->GetComponent<Transform>();

	FrameBuffer frameBuffer;
	frameBuffer.InvView = lightTransform->GetWorldMatrix().GetFastInverse();
	frameBuffer.Projection = lightCam->GetProjectionMatrix();
	frameBuffer.ViewPosition = Math::ToVector4(lightTransform->GetTranslation(true), 1.0f);
	frameBuffer.ViewDirection = Math::ToVector4(lightTransform->GetForwardVector(true), 1.0f);
	frameBuffer.NearPlane = lightCam->GetNearPlane();
	frameBuffer.FarPlane = lightCam->GetFarPlane();
	frameBuffer.Time = { static_cast<float>(Engine::Get().GetTimer().GetTimeSinceProgramStart()), Engine::Get().GetTimer().GetDeltaTime() };
	frameBuffer.Resolution = Engine::Get().GetResolution();
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(frameBuffer);

	QueueObjectShadows(aRenderData, lightCam);
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<EndEvent>();
}

void RenderAssembler::QueueObjectShadows(SceneRenderData& aRenderData, std::shared_ptr<Camera> aRenderCamera)
{
	for (auto& gameObject : aRenderData.castShadowsObjects)
	{
		if (!gameObject->GetActive()) continue;

		std::shared_ptr<Model> model = gameObject->GetComponent<Model>();
		if (model && model->GetActive())
		{
			if (!model->GetShouldViewcull() || IsInsideFrustum(aRenderCamera, gameObject->GetComponent<Transform>(), model->GetBoundingBox()))
			{
				if (aRenderCamera->gameObject->GetName() == "MainCamera")
				{
					UpdateBoundingBox(gameObject);
				}
				else
				{
					if (!model->GetCastShadows()) continue;
				}

				RenderMesh::RenderMeshData data;
				data.mesh = model->GetMesh();
				data.transform = gameObject->GetComponent<Transform>()->GetWorldMatrix();
				data.materialList = model->GetMaterials();
				data.customShaderParams_1 = model->GetCustomShaderData_1();
				data.customShaderParams_2 = model->GetCustomShaderData_2();
				data.psoOverride = AssetManager::Get().GetAsset<PSOAsset>("PSO_Shadow")->pso;
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMesh>(data);
			}
		}

		std::shared_ptr<AnimatedModel> animModel = gameObject->GetComponent<AnimatedModel>();
		if (animModel && animModel->GetActive())
		{
			if (!animModel->GetShouldViewcull() || IsInsideFrustum(aRenderCamera, gameObject->GetComponent<Transform>(), animModel->GetBoundingBox()))
			{
				if (aRenderCamera->gameObject->GetName() == "MainCamera")
				{
					UpdateBoundingBox(gameObject);
				}
				else
				{
					if (!animModel->GetCastShadows()) continue;
				}

				RenderAnimatedMesh::AnimMeshRenderData data;
				data.mesh = animModel->GetMesh();
				data.transform = gameObject->GetComponent<Transform>()->GetWorldMatrix();
				data.materialList = animModel->GetMaterials();
				data.jointTransforms = animModel->GetCurrentPose();
				data.psoOverride = AssetManager::Get().GetAsset<PSOAsset>("PSO_Shadow")->pso;
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMesh>(data);
			}
		}

		std::shared_ptr<InstancedModel> instancedModel = gameObject->GetComponent<InstancedModel>();
		if (instancedModel && instancedModel->GetActive())
		{
			if (!instancedModel->GetShouldViewcull() || IsInsideFrustum(aRenderCamera, gameObject->GetComponent<Transform>(), instancedModel->GetBoundingBox()))
			{
				if (aRenderCamera->gameObject->GetName() == "MainCamera")
				{
					UpdateBoundingBox(gameObject);
				}
				else
				{
					if (!instancedModel->GetCastShadows()) continue;
				}

				RenderInstancedMesh::InstancedMeshRenderData data;
				data.mesh = instancedModel->GetMesh();
				data.transform = gameObject->GetComponent<Transform>()->GetWorldMatrix();
				data.materialList = instancedModel->GetMaterials();
				data.instanceBuffer = &instancedModel->GetInstanceBuffer();
				data.meshCount = instancedModel->GetMeshCount();
				data.psoOverride = AssetManager::Get().GetAsset<PSOAsset>("PSO_Shadow")->pso;
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderInstancedMesh>(data);
			}
		}
	}
}

void RenderAssembler::QueueObjectShadows(SceneRenderData& aRenderData, std::shared_ptr<PointLight> aPointLight)
{
	for (auto& gameObject : aRenderData.castShadowsObjects)
	{
		if (!gameObject->GetActive()) continue;

		std::shared_ptr<Model> model = gameObject->GetComponent<Model>();
		if (model && model->GetActive())
		{
			if (!model->GetCastShadows()) continue;

			if (!model->GetShouldViewcull() || IsInsideRadius(aPointLight, gameObject->GetComponent<Transform>(), model->GetBoundingBox()))
			{
				RenderMesh::RenderMeshData data;
				data.mesh = model->GetMesh();
				data.transform = gameObject->GetComponent<Transform>()->GetWorldMatrix();
				data.materialList = model->GetMaterials();
				data.customShaderParams_1 = model->GetCustomShaderData_1();
				data.customShaderParams_2 = model->GetCustomShaderData_2();
				data.psoOverride = AssetManager::Get().GetAsset<PSOAsset>("PSO_ShadowCube")->pso;
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMesh>(data);
			}
		}

		std::shared_ptr<AnimatedModel> animModel = gameObject->GetComponent<AnimatedModel>();
		if (animModel && animModel->GetActive())
		{
			if (!animModel->GetCastShadows()) continue;

			if (!animModel->GetShouldViewcull() || IsInsideRadius(aPointLight, gameObject->GetComponent<Transform>(), animModel->GetBoundingBox()))
			{
				RenderAnimatedMesh::AnimMeshRenderData data;
				data.mesh = animModel->GetMesh();
				data.transform = gameObject->GetComponent<Transform>()->GetWorldMatrix();
				data.materialList = animModel->GetMaterials();
				data.jointTransforms = animModel->GetCurrentPose();
				data.psoOverride = AssetManager::Get().GetAsset<PSOAsset>("PSO_ShadowCube")->pso;
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMesh>(data);
			}
		}

		std::shared_ptr<InstancedModel> instancedModel = gameObject->GetComponent<InstancedModel>();
		if (instancedModel && instancedModel->GetActive())
		{
			if (!instancedModel->GetCastShadows()) continue;

			if (!instancedModel->GetShouldViewcull() || IsInsideRadius(aPointLight, gameObject->GetComponent<Transform>(), instancedModel->GetBoundingBox()))
			{
				RenderInstancedMesh::InstancedMeshRenderData data;
				data.mesh = instancedModel->GetMesh();
				data.transform = gameObject->GetComponent<Transform>()->GetWorldMatrix();
				data.materialList = instancedModel->GetMaterials();
				data.instanceBuffer = &instancedModel->GetInstanceBuffer();
				data.meshCount = instancedModel->GetMeshCount();
				data.psoOverride = AssetManager::Get().GetAsset<PSOAsset>("PSO_ShadowCube")->pso;
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderInstancedMesh>(data);
			}
		}
	}
}

void RenderAssembler::QueueObjectDebug(SceneRenderData& aRenderData)
{
	std::shared_ptr<PipelineStateObject> pso = AssetManager::Get().GetAsset<PSOAsset>(GraphicsEngine::Get().DebugModeNames[static_cast<int>(GraphicsEngine::Get().CurrentDebugMode)])->pso;

	for (auto& gameObject : aRenderData.drawForward)
	{
		if (!gameObject->GetActive()) continue;
		auto transform = gameObject->GetComponent<Transform>();

		std::shared_ptr<Model> model = gameObject->GetComponent<Model>();
		if (model && model->GetActive())
		{
			if (!model->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, model->GetBoundingBox()))
			{
				if (model->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr)
				{
					RenderMesh::RenderMeshData data;
					data.mesh = model->GetMesh();
					data.transform = transform->GetWorldMatrix();
					data.materialList = model->GetMaterials();
					data.customShaderParams_1 = model->GetCustomShaderData_1();
					data.customShaderParams_2 = model->GetCustomShaderData_2();
					data.psoOverride = pso;

					GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMesh>(data);
				}
			}
		}

		std::shared_ptr<AnimatedModel> animModel = gameObject->GetComponent<AnimatedModel>();
		if (animModel && animModel->GetActive())
		{
			if (!animModel->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, animModel->GetBoundingBox()))
			{
				if (animModel->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr)
				{
					RenderAnimatedMesh::AnimMeshRenderData data;
					data.mesh = animModel->GetMesh();
					data.transform = transform->GetWorldMatrix();
					data.materialList = animModel->GetMaterials();
					data.jointTransforms = animModel->GetCurrentPose();
					data.psoOverride = pso;

					GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMesh>(data);
				}
			}
		}

		std::shared_ptr<InstancedModel> instancedModel = gameObject->GetComponent<InstancedModel>();
		if (instancedModel && instancedModel->GetActive())
		{
			if (!instancedModel->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, instancedModel->GetBoundingBox()))
			{
				if (instancedModel->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr)
				{
					RenderInstancedMesh::InstancedMeshRenderData data;
					data.mesh = instancedModel->GetMesh();
					data.transform = transform->GetWorldMatrix();
					data.materialList = instancedModel->GetMaterials();
					data.instanceBuffer = &instancedModel->GetInstanceBuffer();
					data.meshCount = instancedModel->GetMeshCount();
					data.psoOverride = pso;

					GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderInstancedMesh>(data);
				}
			}
		}
	}

	for (auto& gameObject : aRenderData.drawForward)
	{
		if (!gameObject->GetActive()) continue;
		auto transform = gameObject->GetComponent<Transform>();

		std::shared_ptr<Model> model = gameObject->GetComponent<Model>();
		if (model && model->GetActive())
		{
			if (!model->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, model->GetBoundingBox()))
			{
				if (model->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr)
				{
					RenderMesh::RenderMeshData data;
					data.mesh = model->GetMesh();
					data.transform = transform->GetWorldMatrix();
					data.materialList = model->GetMaterials();
					data.customShaderParams_1 = model->GetCustomShaderData_1();
					data.customShaderParams_2 = model->GetCustomShaderData_2();
					data.psoOverride = pso;

					GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMesh>(data);
				}
			}
		}

		std::shared_ptr<AnimatedModel> animModel = gameObject->GetComponent<AnimatedModel>();
		if (animModel && animModel->GetActive())
		{
			if (!animModel->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, animModel->GetBoundingBox()))
			{
				if (animModel->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr)
				{
					RenderAnimatedMesh::AnimMeshRenderData data;
					data.mesh = animModel->GetMesh();
					data.transform = transform->GetWorldMatrix();
					data.materialList = animModel->GetMaterials();
					data.jointTransforms = animModel->GetCurrentPose();
					data.psoOverride = pso;

					GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMesh>(data);
				}
			}
		}

		std::shared_ptr<InstancedModel> instancedModel = gameObject->GetComponent<InstancedModel>();
		if (instancedModel && instancedModel->GetActive())
		{
			if (!instancedModel->GetShouldViewcull() || IsInsideFrustum(aRenderData.mainCamera, transform, instancedModel->GetBoundingBox()))
			{
				if (instancedModel->GetMaterialOnSlot(0)->GetPSO()->BlendState != nullptr)
				{
					RenderInstancedMesh::InstancedMeshRenderData data;
					data.mesh = instancedModel->GetMesh();
					data.transform = transform->GetWorldMatrix();
					data.materialList = instancedModel->GetMaterials();
					data.instanceBuffer = &instancedModel->GetInstanceBuffer();
					data.meshCount = instancedModel->GetMeshCount();
					data.psoOverride = pso;

					GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderInstancedMesh>(data);
				}
			}
		}
	}
}

void RenderAssembler::QueueDebugLines(SceneRenderData& aRenderData)
{
	if (GraphicsEngine::Get().DrawBoundingBoxes)
	{
		Engine::Get().GetDebugDrawer().DrawBoundingBox(aRenderData.sceneBoundingBox);
		Engine::Get().GetDebugDrawer().DrawBoundingBox(myVisibleObjectsBB);

		for (auto& gameObject : aRenderData.drawBoundingBoxesObjects)
		{
			std::shared_ptr<Model> model = gameObject->GetComponent<Model>();
			if (model && model->GetActive())
			{
				Engine::Get().GetDebugDrawer().DrawBoundingBox(model);
			}
			
			std::shared_ptr<AnimatedModel> animModel = gameObject->GetComponent<AnimatedModel>();
			if (animModel && animModel->GetActive())
			{
				Engine::Get().GetDebugDrawer().DrawBoundingBox(animModel);
			}
			
			std::shared_ptr<InstancedModel> instancedModel = gameObject->GetComponent<InstancedModel>();
			if (instancedModel && instancedModel->GetActive())
			{
				Engine::Get().GetDebugDrawer().DrawBoundingBox(instancedModel->GetBoundingBox());
			}
		}
	}

	if (GraphicsEngine::Get().DrawColliders)
	{
		for (auto& gameObject : aRenderData.drawCollidersObjects)
		{
			std::shared_ptr<BoxCollider> boxCollider = gameObject->GetComponent<BoxCollider>();
			if (boxCollider && boxCollider->GetActive())
			{
				Math::Vector4f colliderColor = boxCollider->debugColliding ? Math::Vector4f(1.0f, 0, 0, 1.0f) : Math::Vector4f(0, 1.0f, 0, 1.0f);
				Engine::Get().GetDebugDrawer().DrawBoundingBox(boxCollider->GetAABB(), gameObject->GetComponent<Transform>()->GetWorldMatrix(), colliderColor);
			}

			std::shared_ptr<SphereCollider> sphereCollider = gameObject->GetComponent<SphereCollider>();
			if (sphereCollider && sphereCollider->GetActive())
			{
				Math::Vector4f colliderColor = sphereCollider->debugColliding ? Math::Vector4f(1.0f, 0, 0, 1.0f) : Math::Vector4f(0, 1.0f, 0, 1.0f);
				Engine::Get().GetDebugDrawer().DrawBoundingSphere(sphereCollider->GetSphere(), gameObject->GetComponent<Transform>()->GetWorldMatrix(), colliderColor);
			}
		}
	}

	if (GraphicsEngine::Get().DrawCameraFrustums)
	{
		for (auto& gameObject : aRenderData.drawCameraFrustumsObjects)
		{
			std::shared_ptr<Camera> cam = gameObject->GetComponent<Camera>();
			if (cam && cam->GetActive() && cam != aRenderData.mainCamera)
			{
				Engine::Get().GetDebugDrawer().DrawCameraFrustum(cam);
			}
		}
	}
}

bool RenderAssembler::IsInsideFrustum(std::shared_ptr<Camera> aRenderCamera, std::shared_ptr<Transform> aObjectTransform, Math::AABB3D<float> aObjectAABB)
{
	if (!GraphicsEngine::Get().UseViewCulling) return true;

	return aRenderCamera->GetViewcullingIntersection(aObjectTransform, aObjectAABB);
}

bool RenderAssembler::IsInsideRadius(std::shared_ptr<PointLight> aPointLight, std::shared_ptr<Transform> aObjectTransform, Math::AABB3D<float> aObjectAABB)
{
	if (!GraphicsEngine::Get().UseViewCulling) return true;

	std::shared_ptr<Transform> pointLightTransform = aPointLight->gameObject->GetComponent<Transform>();
	std::shared_ptr<Camera> pointLightCam = aPointLight->gameObject->GetComponent<Camera>();
	if (!pointLightTransform) return true;

	Math::Matrix4x4f objectMatrix = aObjectTransform->GetWorldMatrix();

	if (aObjectTransform->IsScaled())
	{
		objectMatrix = objectMatrix.GetInverse();
	}
	else
	{
		objectMatrix = objectMatrix.GetFastInverse();
	}

	Math::Sphere<float> sphere(pointLightTransform->GetTranslation(), pointLightCam->GetFarPlane());
	sphere = sphere.GetSphereinNewSpace(pointLightTransform->GetWorldMatrix() * objectMatrix);
	return Math::IntersectionSphereAABB(sphere, aObjectAABB);
}

void RenderAssembler::UpdateBoundingBox(std::shared_ptr<GameObject> aGameObject)
{
	if (aGameObject->GetComponent<Transform>())
	{
		Math::Vector3f bbMin = myVisibleObjectsBB.GetMin();
		Math::Vector3f bbMax = myVisibleObjectsBB.GetMax();
		std::shared_ptr<Transform> objectTransform = aGameObject->GetComponent<Transform>();

		std::shared_ptr<Model> model = aGameObject->GetComponent<Model>();
		std::shared_ptr<AnimatedModel> animModel = aGameObject->GetComponent<AnimatedModel>();
		std::shared_ptr<InstancedModel> instancedModel = aGameObject->GetComponent<InstancedModel>();
		if (model)
		{
			if (!model->GetShouldViewcull()) return;

			auto& corners = model->GetBoundingBox().GetCorners();

			for (Math::Vector3f corner : corners)
			{
				corner = Math::ToVector3(Math::ToVector4(corner, 1.0f) * objectTransform->GetWorldMatrix());

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
			for (Math::Vector3f corner : corners)
			{
				corner = Math::ToVector3(Math::ToVector4(corner, 1.0f) * objectTransform->GetWorldMatrix());

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
			for (Math::Vector3f corner : corners)
			{
				corner = Math::ToVector3(Math::ToVector4(corner, 1.0f) * objectTransform->GetWorldMatrix());

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
			Math::Vector3f point = objectTransform->GetTranslation(true);

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
void RenderAssembler::Init()
{
	//myTestSprite = std::make_shared<Sprite>();
	//myTestText = std::make_shared<Text>();

	//myTestText->SetFont(AssetManager::Get().GetAsset<FontAsset>("Fonts/F_RobotoRegular.json")->font);
	//myTestText->SetPosition(Math::Vector2f(-500.0f, 700.0f));
	//myTestText->SetSize(5);
	//myTestText->SetTextContent("Test");
}

void RenderAssembler::DrawTestUI()
{
	//myTestSprite->SetTexture(AssetManager::Get().GetAsset<TextureAsset>("EngineAssets/Textures/CommonUtilitiesT_perlin_C.dds")->texture);
	//myTestSprite->SetPosition(Math::Vector2f(500.0f, 500.0f));
	//myTestSprite->SetSize(Math::Vector2f(600.0f, 600.0f));
	//GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(GraphicsEngine::Get().GetBackBuffer(), nullptr, false, false);
	// //GraphicsEngine::Get().ChangePipelineState(AssetManager::Get().GetAsset<PSOAsset>("PSO_Sprite")->pso);
	// //GraphicsEngine::Get().ChangePipelineState(AssetManager::Get().GetAsset<PSOAsset>("PSO_Spritesheet")->pso);
	//GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderSprite>(myTestSprite);

	//GraphicsEngine::Get().ChangePipelineState(AssetManager::Get().GetAsset<PSOAsset>("PSO_Text")->pso);
	//GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderText>(myTestText);
}