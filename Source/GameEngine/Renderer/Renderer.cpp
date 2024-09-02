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
	GraphicsEngine& gfx = GraphicsEngine::Get();

	if (gfx.RecalculateShadowFrustum)
	{
		aScene.myDirectionalLight->GetComponent<DirectionalLight>()->RecalculateShadowFrustum(aScene.myMainCamera, myVisibleObjectsBB);
		myVisibleObjectsBB.InitWithCenterAndExtents(CU::Vector3f(), CU::Vector3f());
	}

	QueueDirectionalLightShadows(aScene);
	QueuePointLightShadows(aScene);
	QueueSpotLightShadows(aScene);
	QueueUpdateLightBuffer(aScene);

	// Final Render
	gfx.GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_PBR")->pso);
	gfx.GetGraphicsCommandList().Enqueue<SetDefaultRenderTarget>();
	gfx.GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(aScene.myMainCamera->GetComponent<Camera>());
	gfx.GetGraphicsCommandList().Enqueue<SetTextureResource>(126, aScene.myAmbientLight->GetComponent<AmbientLight>()->GetCubemap());
	QueueShadowmapTextureResources(aScene);


	if (gfx.GetCurrentDebugMode() != DebugMode::None)
	{
		std::shared_ptr<PipelineStateObject> pso = AssetManager::Get().GetAsset<PSOAsset>(GraphicsEngine::Get().DebugModeNames[static_cast<int>(GraphicsEngine::Get().GetCurrentDebugMode())])->pso;
		QueueGameObjects(aScene, aScene.myMainCamera->GetComponent<Camera>(), false, pso);
	}
	else
	{
		QueueGameObjects(aScene, aScene.myMainCamera->GetComponent<Camera>());
	}

	if (gfx.DrawGizmos)
	{
		QueueDebugGizmos(aScene, aScene.myMainCamera->GetComponent<Camera>());
	}

	QueueDebugLines(aScene);

	QueueClearTextureResources();

	DrawTestUI();
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

		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_Shadow")->pso);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, spotLight->GetShadowMap(), false, true);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(aScene.mySpotLights[i]->GetComponent<Camera>());
		QueueGameObjects(aScene, aScene.mySpotLights[i]->GetComponent<Camera>(), false, AssetManager::Get().GetAsset<PSOAsset>("PSO_Shadow")->pso);
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
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_ShadowCube")->pso);
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(aScene.myPointLights[i]->GetComponent<Camera>());
		GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateShadowBuffer>(pointLight);
		QueueGameObjects(aScene, pointLight, false, AssetManager::Get().GetAsset<PSOAsset>("PSO_ShadowCube")->pso);
	}
}

void Renderer::QueueDirectionalLightShadows(Scene& aScene)
{
	std::shared_ptr<DirectionalLight> dLight = aScene.myDirectionalLight->GetComponent<DirectionalLight>();
	if (!dLight->GetActive()) return;
	if (!dLight->CastsShadows()) return;

	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<SetRenderTarget>(nullptr, dLight->GetShadowMap(), false, true);
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<ChangePipelineState>(AssetManager::Get().GetAsset<PSOAsset>("PSO_Shadow")->pso);
	GraphicsEngine::Get().GetGraphicsCommandList().Enqueue<UpdateFrameBuffer>(aScene.myDirectionalLight->GetComponent<Camera>());
	QueueGameObjects(aScene, aScene.myDirectionalLight->GetComponent<Camera>(), false, AssetManager::Get().GetAsset<PSOAsset>("PSO_Shadow")->pso);
}

void Renderer::QueueDebugGizmos(Scene& aScene, std::shared_ptr<Camera> aRenderCamera)
{
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