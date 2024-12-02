#include "Enginepch.h"
#include "CollisionAvoidance.h"

#include "GameEngine/Engine.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Camera.h"
#include "Movement2/AI/PollingStation.h"

ControllerBase::SteeringOutput CollisionAvoidance::GetSteering(const SteeringInput& aSteeringInput)
{
    SteeringOutput output;
    output.velocity = aSteeringInput.velocity;

	auto viewport = Engine::GetInstance().GetSceneHandler().FindGameObjectByName("MainCamera")->GetComponent<Camera>()->GetViewportDimensions();

	if (aSteeringInput.position.x < -viewport.x + myEdgeMargin)
	{
		output.velocity.x = aSteeringInput.velocity.x + myTurnFactor;
	}
	else if (aSteeringInput.position.x > viewport.x - myEdgeMargin)
	{
		output.velocity.x = aSteeringInput.velocity.x - myTurnFactor;
	}	

	if (aSteeringInput.position.z < -viewport.y + myEdgeMargin)
	{
		output.velocity.z = aSteeringInput.velocity.z + myTurnFactor;
	}
	else if (aSteeringInput.position.z > viewport.y - myEdgeMargin)
	{
		output.velocity.z = aSteeringInput.velocity.z - myTurnFactor;
	}

    return output;
}