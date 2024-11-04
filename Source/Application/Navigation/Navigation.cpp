#include "Enginepch.h"
#include "Navigation.h"
#include <GameEngine/Engine.h>
#include "GameEngine/Pathfinding/NavMesh.h"
#include "GameEngine/Pathfinding/Components/NavMeshAgent.h"

#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Camera.h"
#include "GameEngine/ComponentSystem/Components/Physics/Colliders/BoxCollider.h"
#include "Intersections/Intersection3D.hpp"
#include "WalkToPoint.h"

Application* CreateApplication()
{
	Engine::GetInstance().LoadSettings(std::filesystem::current_path().string() + "/" + APP_SETTINGS_PATH);
    return new Navigation();
}

void Navigation::InitializeApplication()
{
	InputHandler& inputHandler = Engine::GetInstance().GetInputHandler();

	inputHandler.RegisterBinaryAction("LMB", Keys::MOUSELBUTTON, GenericInput::ActionType::Held);
	inputHandler.RegisterAnalog2DAction("MousePosNDC", MouseMovement2D::MousePosNDC);

	GraphicsEngine::Get().RecalculateShadowFrustum = false;
	Engine::GetInstance().GetSceneHandler().LoadScene("Scenes/SC_Navigation.json");

	myNavMesh = AssetManager::Get().GetAsset<NavMeshAsset>("NM_Navmesh")->navmesh;
	Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Companion")->AddComponent<NavMeshAgent>(myNavMesh.get(), 150.0f);
	Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Player")->AddComponent<WalkToPoint>(200.0f);
}

void Navigation::UpdateApplication()
{
	myNavMesh->DrawDebugLines();
	myNavMesh->DrawBoundingBox();
	myNavMesh->DrawFunnelLines();

	if (Engine::GetInstance().GetInputHandler().GetBinaryAction("LMB"))
	{
		myNavMesh->ClearFunnelLines();
		CastRay();
	}

	Engine::GetInstance().GetDebugDrawer().DrawLine(myDebugRay);
}

void Navigation::CastRay()
{
	CU::Vector2f mousePosNDC = Engine::GetInstance().GetInputHandler().GetAnalogAction2D("MousePosNDC");

	auto cam = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("MainCamera")->GetComponent<Camera>();
	CU::Matrix4x4f invProj = cam->GetProjectionMatrix().GetInverse();
	CU::Vector4f eyeCoords = CU::Vector4f{ mousePosNDC.x, mousePosNDC.y, 0.0f, 1.0f } *invProj;

	eyeCoords /= eyeCoords.w;

	auto camTransform = cam->gameObject->GetComponent<Transform>();
	CU::Matrix4x4f camView = camTransform->GetWorldMatrix();
	CU::Vector4f rayWorld = eyeCoords * camView;

	CU::Vector3f mousePosWorld = { rayWorld.x, rayWorld.y, rayWorld.z };

	const CU::Vector3f cameraPos = camTransform->GetTranslation(true);
	const CU::Vector3f direction = mousePosWorld - cameraPos;

	CU::Ray<float> mouseRay(cameraPos, direction.GetNormalized());

	if (auto collider = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Plane")->GetComponent<BoxCollider>())
	{
		CU::Vector3f hitPoint;
		bool hit = CU::IntersectionAABBRay(collider->GetAABB(), mouseRay, hitPoint);

		if (hit)
		{
			myDebugRay.From = mouseRay.GetOrigin();
			myDebugRay.To = hitPoint;
			myDebugRay.Color = { 0, 1.0f, 0, 1.0f };
			Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Player")->GetComponent<WalkToPoint>()->SetTarget(hitPoint);
			Engine::GetInstance().GetSceneHandler().FindGameObjectByName("Companion")->GetComponent<NavMeshAgent>()->MoveToLocation(hitPoint);
		}
		else
		{
			myDebugRay.From = mouseRay.GetOrigin();
			myDebugRay.To = mouseRay.GetOrigin() + mouseRay.GetDirection() * 1000.0f;
			myDebugRay.Color = { 1.0f, 0, 0, 1.0f };
		}
	}
}
