#pragma once
#include "ComponentSystem/Component.h"
#include <Math/Vector.hpp>
#include "DebugDrawer/DebugLine.hpp"
#include "Pathfinding/NavMeshPath.h"

class NavMesh;

class NavMeshAgent : public Component
{
public:
	NavMeshAgent() = default;
	NavMeshAgent(NavMesh* aNavMesh, float aMovementSpeed);

	void Start() override;
	void Update() override;

	void SetNavMesh(NavMesh* aNavMesh);
	void SetMovementSpeed(float aSpeed);
	float GetMovementSpeed() const;

	void MoveToLocation(Math::Vector3f aPosition);
	void Stop();

private:
	bool MoveToNextPathPoint();
	void RotateTowardsVelocity(Math::Vector3f aDirection);

	NavMesh* myNavMesh;
	float myMovementSpeed = 150.0f;
	float myRotationSpeed = 20.0f;

	float myCurrentRotationTime = 0;
	float myMaxRotationTime = 1.0f;

	Math::Quatf myStartRotation;
	Math::Quatf myGoalRotation;

	bool myShouldPathfind = false;
	NavMeshPath myPath;
	int myCurrentGoalPoint = 0;
	float myGoalTolerance = 40.0f;

#ifndef _RETAIL
	void CreateDebugPath();
	void RenderDebugPath();
	std::vector<DebugLine> myPathLines;
#endif
};

