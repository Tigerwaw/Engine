#include "Enginepch.h"
#include "NavMeshAgent.h"
#include "Pathfinding/NavMesh.h"

#include "Engine.h"
#include "Time/Timer.h"

#include "DebugDrawer/DebugDrawer.h"
#include "Math/Intersection3D.hpp"

#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"

NavMeshAgent::NavMeshAgent(NavMesh* aNavMesh, float aMovementSpeed)
{
    myNavMesh = aNavMesh;
    SetMovementSpeed(aMovementSpeed);
}

void NavMeshAgent::Start()
{
    SetNavMesh(myNavMesh);
}

void NavMeshAgent::Update()
{
    if (myShouldPathfind)
    {
        MoveToNextPathPoint();

#ifndef _RETAIL
        RenderDebugPath();
#endif
    }
}

void NavMeshAgent::SetNavMesh(NavMesh* aNavMesh)
{
    myNavMesh = aNavMesh;
    if (myNavMesh)
    {
        myNavMesh->SnapGameObjectToNavMesh(*gameObject);
    }
}

void NavMeshAgent::SetMovementSpeed(float aSpeed)
{
    myMovementSpeed = aSpeed;
}

float NavMeshAgent::GetMovementSpeed() const
{
    return myMovementSpeed;
}

void NavMeshAgent::MoveToLocation(Math::Vector3f aPosition)
{
    if (!myNavMesh)
    {
        LOG(LogComponentSystem, Warning, "Navigation agent {} does not have a reference to the navmesh!", gameObject->GetName());
        return;
    }
    NavMeshPath navPath = myNavMesh->FindPath(gameObject->GetComponent<Transform>()->GetTranslation(), aPosition);
    if (navPath.Empty()) return;

    myPath = navPath;
    myCurrentGoalPoint = 0;
    myShouldPathfind = true;

#ifndef _RETAIL
    CreateDebugPath();
#endif
}

void NavMeshAgent::Stop()
{
    myShouldPathfind = false;
}

bool NavMeshAgent::MoveToNextPathPoint()
{
    auto transform = gameObject->GetComponent<Transform>();

    Math::Vector3f position = transform->GetTranslation();
    float distanceToNextVertex = (myPath[myCurrentGoalPoint] - position).LengthSqr();

    if (distanceToNextVertex < myGoalTolerance)
    {
        myCurrentGoalPoint++;

        if (myCurrentGoalPoint >= myPath.GetSize())
        {
            myShouldPathfind = false;
            return false;
        }
    }

    Math::Vector3f direction = (myPath[myCurrentGoalPoint] - position).GetNormalized();
    Math::Vector3f moveDelta = direction * myMovementSpeed * Engine::Get().GetTimer().GetDeltaTime();
    transform->SetTranslation(transform->GetTranslation() + moveDelta);

    RotateTowardsVelocity(direction);

    return true;
}

void NavMeshAgent::RotateTowardsVelocity(Math::Vector3f aDirection)
{
    myCurrentRotationTime += Engine::Get().GetTimer().GetDeltaTime();
    float t = myCurrentRotationTime / myMaxRotationTime;

    Math::Quatf slerpedRotation = Math::Quatf::Slerp(myStartRotation, myGoalRotation, t);
    gameObject->GetComponent<Transform>()->SetRotation(slerpedRotation.GetEulerAnglesDegrees());

    if (t >= 0.99f)
    {
        myCurrentRotationTime = 0;
        myMaxRotationTime = 1 / myRotationSpeed;

        myStartRotation = myGoalRotation;
        myGoalRotation = Math::Quatf(Math::Vector3f(0, atan2(aDirection.x, aDirection.z), 0));
    }
}

#ifndef _RETAIL
void NavMeshAgent::CreateDebugPath()
{
    myPathLines.clear();

    for (int i = 0; i < static_cast<int>(myPath.GetSize()) - 1; i++)
    {
        Math::Vector3f offset = { 0, 15.0f, 0 };
        DebugLine pathLine;
        pathLine.Color = { 1.0f, 0.0f, 0.0f, 1.0f };
        pathLine.From = myPath[i] + offset;
        pathLine.To = myPath[i + 1] + offset;
        myPathLines.emplace_back(pathLine);
    }
}

void NavMeshAgent::RenderDebugPath()
{
    for (auto& line : myPathLines)
    {
        Engine::Get().GetDebugDrawer().DrawLine(line);
    }
}
#endif
