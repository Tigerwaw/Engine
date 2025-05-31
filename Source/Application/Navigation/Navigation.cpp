#include "Enginepch.h"
#include "Navigation.h"
#include <Application/AppSettings.h>
#include <Engine.h>
#include <Pathfinding/NavMesh.h>
#include <Pathfinding/Components/NavMeshAgent.h>

#include <SceneHandler/SceneHandler.h>
#include <ComponentSystem/GameObject.h>
#include <ComponentSystem/Components/Transform.h>
#include <ComponentSystem/Components/Graphics/Camera.h>
#include <ComponentSystem/Components/Physics/Colliders/BoxCollider.h>
#include <Math/Intersection3D.hpp>
#include "WalkToPoint.h"

Application* CreateApplication()
{
	AppSettings::LoadSettings(std::filesystem::current_path() / APP_SETTINGS_PATH);
    return new Navigation();
}

void Navigation::InitializeApplication()
{
	InputHandler& inputHandler = Engine::Get().GetInputHandler();

	inputHandler.RegisterBinaryAction("LMB", Keys::MOUSELBUTTON, GenericInput::ActionType::Held);
	inputHandler.RegisterAnalog2DAction("MousePosNDC", MouseMovement2D::MousePosNDC);

	GraphicsEngine::Get().RecalculateShadowFrustum = false;
	Engine::Get().GetSceneHandler().LoadScene("Scenes/SC_Navigation.json");

	myNavMesh = AssetManager::Get().GetAsset<NavMeshAsset>("NM_Navmesh")->navmesh;
	Engine::Get().GetSceneHandler().FindGameObjectByName("Companion")->AddComponent<NavMeshAgent>(myNavMesh.get(), 150.0f);
	Engine::Get().GetSceneHandler().FindGameObjectByName("Player")->AddComponent<WalkToPoint>(200.0f);
}

void Navigation::UpdateApplication()
{
#ifndef _RETAIL
	myNavMesh->DrawDebugLines();
	myNavMesh->DrawBoundingBox();
#endif

	if (Engine::Get().GetInputHandler().GetBinaryAction("LMB"))
	{
		CastRay();
	}

#ifndef _RETAIL
	Engine::Get().GetDebugDrawer().DrawLine(myDebugRay);
#endif
}

void Navigation::CastRay()
{
	Math::Vector2f mousePosNDC = Engine::Get().GetInputHandler().GetAnalogAction2D("MousePosNDC");

	auto cam = Engine::Get().GetSceneHandler().FindGameObjectByName("MainCamera")->GetComponent<Camera>();
	Math::Matrix4x4f invProj = cam->GetProjectionMatrix().GetInverse();
	Math::Vector4f eyeCoords = Math::Vector4f{ mousePosNDC.x, mousePosNDC.y, 0.0f, 1.0f } *invProj;

	eyeCoords /= eyeCoords.w;

	auto camTransform = cam->gameObject->GetComponent<Transform>();
	Math::Matrix4x4f camView = camTransform->GetWorldMatrix();
	Math::Vector4f rayWorld = eyeCoords * camView;

	Math::Vector3f mousePosWorld = { rayWorld.x, rayWorld.y, rayWorld.z };

	const Math::Vector3f cameraPos = camTransform->GetTranslation(true);
	const Math::Vector3f direction = mousePosWorld - cameraPos;

	Math::Ray<float> mouseRay(cameraPos, direction.GetNormalized());

	if (auto collider = Engine::Get().GetSceneHandler().FindGameObjectByName("Plane")->GetComponent<BoxCollider>())
	{
		Math::Vector3f hitPoint;
		bool hit = Math::IntersectionAABBRay(collider->GetAABB(), mouseRay, hitPoint);

		if (hit)
		{
			myDebugRay.From = mouseRay.GetOrigin();
			myDebugRay.To = hitPoint;
			myDebugRay.Color = { 0, 1.0f, 0, 1.0f };
			Engine::Get().GetSceneHandler().FindGameObjectByName("Player")->GetComponent<WalkToPoint>()->SetTarget(hitPoint);
			Engine::Get().GetSceneHandler().FindGameObjectByName("Companion")->GetComponent<NavMeshAgent>()->MoveToLocation(hitPoint);
		}
		else
		{
			myDebugRay.From = mouseRay.GetOrigin();
			myDebugRay.To = mouseRay.GetOrigin() + mouseRay.GetDirection() * 1000.0f;
			myDebugRay.Color = { 1.0f, 0, 0, 1.0f };
		}
	}
}
