#include "Enginepch.h"
#include "Collider.h"

void Collider::SetCollisionResponse(const std::function<void()>& aCallback)
{
	myCollisionResponse = aCallback;
}

void Collider::TriggerCollisionResponse() const
{
	if (myCollisionResponse)
	{
		myCollisionResponse();
	}
}
