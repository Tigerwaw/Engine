#include "Enginepch.h"
#include "NavMeshAgent.h"
#include "Pathfinding/NavMesh.h"

#include "GameEngine/Engine.h"
#include "GameEngine/Time/Timer.h"

#include "GameEngine/DebugDrawer/DebugDrawer.h"
#include "GameEngine/Intersections/Intersection3D.hpp"

#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"

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

#ifdef _DEBUG
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

void NavMeshAgent::MoveToLocation(CU::Vector3f aPosition)
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

#ifdef _DEBUG
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

    CU::Vector3f position = transform->GetTranslation();
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

    CU::Vector3f direction = (myPath[myCurrentGoalPoint] - position).GetNormalized();
    CU::Vector3f moveDelta = direction * myMovementSpeed * Engine::GetInstance().GetTimer().GetDeltaTime();
    transform->SetTranslation(transform->GetTranslation() + moveDelta);

    RotateTowardsVelocity(direction);

    return true;
}

void NavMeshAgent::RotateTowardsVelocity(CU::Vector3f aDirection)
{
    myCurrentRotationTime += Engine::GetInstance().GetTimer().GetDeltaTime();
    float t = myCurrentRotationTime / myMaxRotationTime;

    CU::Quatf slerpedRotation = CU::Quatf::Slerp(myStartRotation, myGoalRotation, t);
    gameObject->GetComponent<Transform>()->SetRotation(slerpedRotation.GetEulerAnglesDegrees());

    if (t >= 0.99f)
    {
        myCurrentRotationTime = 0;
        myMaxRotationTime = 1 / myRotationSpeed;

        myStartRotation = myGoalRotation;
        myGoalRotation = CU::Quatf(CU::Vector3f(0, atan2(aDirection.x, aDirection.z), 0));
    }
}

#ifdef _DEBUG
void NavMeshAgent::CreateDebugPath()
{
    myPathLines.clear();

    for (int i = 0; i < static_cast<int>(myPath.GetSize()) - 1; i++)
    {
        CU::Vector3f offset = { 0, 15.0f, 0 };
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
        Engine::GetInstance().GetDebugDrawer().DrawLine(line);
    }
}
#endif
