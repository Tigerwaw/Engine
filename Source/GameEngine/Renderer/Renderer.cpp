#include "Enginepch.h"
#include "Renderer.h"

#include "GameEngine/Engine.h"
#include "GameEngine/DebugDrawer/DebugDrawer.h"
#include "AssetManager/AssetManager.h"
#include "GraphicsEngine/GraphicsEngine.h"
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/Intersections/AABB3D.hpp"
#include "GameEngine/Intersections/Intersection3D.hpp"
namespace CU = CommonUtilities;

#include "GameEngine/ComponentSystem/Scene.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Model.h"
#include "GameEngine/ComponentSystem/Components/Graphics/AnimatedModel.h"
#include "GameEngine/ComponentSystem/Components/Graphics/DebugModel.h"
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
	Engine::GetInstance().GetDebugDrawer().DrawBoundingBox(aScene.myBoundingBox);
	aScene.myDirectionalLight->GetComponent<DirectionalLight>()->RecalculateShadowFrustum(aScene.myMainCamera, aScene.myBoundingBox);
	QueueDirectionalLightShadows(aScene);
	QueuePointLightShadows(aScene);
	QueueSpotLightShadows(aScene);
	QueueUpdateLightBuffer(aScene);

	// Final Render
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PBR")->pso);
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetDefaultRenderTarget>();
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(aScene.myMainCamera->GetComponent<Camera>());
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetTextureResource>(126, aScene.myAmbientLight->GetComponent<AmbientLight>()->GetCubemap());
	QueueShadowmapTextureResources(aScene);


	if (GraphicsEngine::Get().GetCurrentDebugMode() != DebugMode::None)
	{
		std::shared_ptr<PipelineStateObject> pso = AssetManager::Get().GetAsset<PSOAsset>(GraphicsEngine::Get().DebugModeNames[static_cast<int>(GraphicsEngine::Get().GetCurrentDebugMode())])->pso;
		QueueGameObjects(aScene, aScene.myMainCamera->GetComponent<Camera>(), false, pso);
	}
	else
	{
		QueueGameObjects(aScene, aScene.myMainCamera->GetComponent<Camera>());
	}

	if (GraphicsEngine::Get().DrawGizmos)
	{
		QueueDebugGizmos(aScene, aScene.myMainCamera->GetComponent<Camera>());
	}

	QueueDebugLines(aScene);

	QueueClearTextureResources();
}

void Renderer::QueueClearTextureResources()
{
	for (int i = 100; i < 110; i++)
	{
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ClearTextureResource>(i);
	}
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ClearTextureResource>(126);
}

void Renderer::QueueShadowmapTextureResources(Scene& aScene)
{
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
	for (int i = 0; i < aScene.mySpotLights.size(); i++)
	{
		std::shared_ptr<SpotLight> spotLight = aScene.mySpotLights[i]->GetComponent<SpotLight>();
		if (!spotLight->GetActive()) continue;
		if (!spotLight->CastsShadows()) continue;

		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("Shadow")->pso);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, spotLight->GetShadowMap(), false, true);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(aScene.mySpotLights[i]->GetComponent<Camera>());
		QueueGameObjects(aScene, aScene.mySpotLights[i]->GetComponent<Camera>(), false, AssetManager::Get().GetAsset<PSOAsset>("Shadow")->pso);
	}
}

void Renderer::QueuePointLightShadows(Scene& aScene)
{
	for (int i = 0; i < aScene.myPointLights.size(); i++)
	{
		std::shared_ptr<PointLight> pointLight = aScene.myPointLights[i]->GetComponent<PointLight>();
		if (!pointLight->GetActive()) continue;
		if (!pointLight->CastsShadows()) continue;

		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, pointLight->GetShadowMap(), false, true);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("ShadowCube")->pso);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(aScene.myPointLights[i]->GetComponent<Camera>());
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateShadowBuffer>(pointLight);
		QueueGameObjects(aScene, pointLight, false, AssetManager::Get().GetAsset<PSOAsset>("ShadowCube")->pso);
	}
}

void Renderer::QueueDirectionalLightShadows(Scene& aScene)
{
	std::shared_ptr<DirectionalLight> dLight = aScene.myDirectionalLight->GetComponent<DirectionalLight>();
	if (!dLight->GetActive()) return;
	if (!dLight->CastsShadows()) return;

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, dLight->GetShadowMap(), false, true);
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("Shadow")->pso);
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(aScene.myDirectionalLight->GetComponent<Camera>());
	QueueGameObjects(aScene, aScene.myDirectionalLight->GetComponent<Camera>(), false, AssetManager::Get().GetAsset<PSOAsset>("Shadow")->pso);
}

void Renderer::QueueDebugGizmos(Scene& aScene, std::shared_ptr<Camera> aRenderCamera)
{
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("Gizmo")->pso);
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
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMesh>(model, aPSOoverride);
			}
		}

		std::shared_ptr<AnimatedModel> animModel = gameObject->GetComponent<AnimatedModel>();
		if (animModel && animModel->GetActive())
		{
			if (aDisableViewCulling || !animModel->GetShouldViewcull() || IsInsideFrustum(aRenderCamera, gameObject->GetComponent<Transform>(), animModel->GetBoundingBox()))
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMesh>(animModel, aPSOoverride);
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
			if (aDisableViewCulling || !model->GetShouldViewcull() || IsInsideRadius(aPointLight, gameObject->GetComponent<Transform>(), model->GetBoundingBox()))
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderMesh>(model, aPSOoverride);
			}
		}

		std::shared_ptr<AnimatedModel> animModel = gameObject->GetComponent<AnimatedModel>();
		if (animModel && animModel->GetActive())
		{
			if (aDisableViewCulling || !animModel->GetShouldViewcull() || IsInsideRadius(aPointLight, gameObject->GetComponent<Transform>(), animModel->GetBoundingBox()))
			{
				GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<RenderAnimatedMesh>(animModel, aPSOoverride);
			}
		}
	}
}

void Renderer::QueueDebugLines(Scene& aScene)
{
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
