#include "Enginepch.h"
#include "RandomDirectionMovement.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "Engine.h"
#include "Time/Timer.h"

void RandomDirectionMovement::Start()
{
	myDirection.x = static_cast<float>((rand() % 10000) - 5000);
	myDirection.z = static_cast<float>((rand() % 10000) - 5000);
	myDirection.Normalize();
}

void RandomDirectionMovement::Update()
{
	if (auto transform = gameObject->GetComponent<Transform>())
	{
		transform->AddTranslation(myDirection * mySpeed * Engine::Get().GetTimer().GetDeltaTime());
	}
}

void RandomDirectionMovement::SetDirection(CU::Vector3f aNewDirection)
{
	myDirection = aNewDirection;
	myDirection.y = 0.0f;
	myDirection.Normalize();
}

const CU::Vector3f& RandomDirectionMovement::GetDirection() const
{
	return myDirection;
}
