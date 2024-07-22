#include "Transform.h"

Transform::Transform(CU::Vector3f aPosition, CU::Vector3f aRotation, CU::Vector3f aScale)
{
	myPosition = aPosition;
	myRotation = aRotation;
	myScale = aScale;
}

void Transform::Start()
{

}

void Transform::Update()
{

}

void Transform::UpdateToWorldMatrix(CU::Matrix4x4f aToWorldMatrix, CU::Matrix4x4f aToWorldMatrixNoScale)
{
	myToWorldMatrix = aToWorldMatrix;
	myToWorldMatrixNoScale = aToWorldMatrixNoScale;

	for (auto& child : myChildren)
	{
		child->UpdateToWorldMatrix(aToWorldMatrix * GetMatrix().GetFastInverse(), aToWorldMatrixNoScale * GetMatrix(true).GetFastInverse());
	}
}

void Transform::SetIsDirty()
{
	myIsDirty = true;

	if (!myParent)
	{
		UpdateToWorldMatrix(GetMatrix().GetFastInverse(), GetMatrix(true).GetFastInverse());
	}
}

const CU::Matrix4x4f& Transform::GetMatrix(bool aNoScale)
{
	if (myIsDirty)
	{
		myCachedMatrix = CU::Matrix4x4f();
		myCachedMatrixNoScale = CU::Matrix4x4f();

		CU::Matrix4x4f scaleMatrix;
		scaleMatrix(1, 1) = myScale.x;
		scaleMatrix(2, 2) = myScale.y;
		scaleMatrix(3, 3) = myScale.z;

		myCachedMatrix *= scaleMatrix;

		CU::Matrix4x4f rotationMatrix = CU::Matrix4x4f::CreateRollPitchYawMatrix(myRotation);
		myCachedMatrix *= rotationMatrix;
		myCachedMatrixNoScale *= rotationMatrix;

		CU::Matrix4x4f translationMatrix;
		translationMatrix(4, 1) = myPosition.x;
		translationMatrix(4, 2) = myPosition.y;
		translationMatrix(4, 3) = myPosition.z;

		myCachedMatrix *= translationMatrix;
		myCachedMatrixNoScale *= translationMatrix;

		myIsDirty = false;
	}

	if (aNoScale)
	{
		return myCachedMatrixNoScale;
	}
	else
	{
		return myCachedMatrix;
	}
}

const CU::Matrix4x4f Transform::GetWorldMatrix(bool aNoScale)
{
	if (aNoScale)
	{
		return GetMatrix(true) * myToWorldMatrixNoScale;
	}
	else
	{
		return GetMatrix(false) * myToWorldMatrix;
	}
}

void Transform::SetParent(Transform* aTransform)
{
	if (myParent)
	{
		myParent->RemoveChild(this);
		myParent = aTransform;
	}
	else
	{
		myParent = aTransform;
	}
}

void Transform::AddChild(Transform* aTransform)
{
	for (int i = 0; i < myChildren.size(); i++)
	{
		if (myChildren.empty()) break;
		if (myChildren[i] == aTransform) return;
	}

	myChildren.emplace_back(aTransform);
}

void Transform::RemoveChild(Transform* aTransform)
{
	if (myChildren.empty()) return;

	for (int i = 0; i < myChildren.size(); i++)
	{
		if (myChildren[i] == aTransform)
		{
			myChildren.erase(myChildren.begin() + i);
			break;
		}
	}
}

const CU::Vector3f Transform::GetRightVector()
{
	return GetMatrix().GetRightVector();
}

const CU::Vector3f Transform::GetUpVector()
{
	return GetMatrix().GetUpVector();
}

const CU::Vector3f Transform::GetForwardVector()
{
	return GetMatrix().GetForwardVector();
}

void Transform::SetTranslation(const CU::Vector3f aTranslation)
{
	myPosition = aTranslation;
	myIsDirty = true;
}

void Transform::SetTranslation(const float aX, const float aY, const float aZ)
{
	myPosition = { aX, aY, aZ };
	myIsDirty = true;
}

void Transform::SetRotation(const CU::Vector3f aRotationInDegrees)
{
	myRotation = aRotationInDegrees;
	myIsDirty = true;
}

void Transform::SetRotation(const float aPitch, const float aYaw, const float aRoll)
{
	myRotation.x = aPitch;
	myRotation.y = aYaw;
	myRotation.z = aRoll;
	myIsDirty = true;
}

void Transform::AddRotation(const CU::Vector3f aRotationInDegrees)
{
	myRotation += aRotationInDegrees;
	myIsDirty = true;
}

void Transform::AddRotation(const float aPitch, const float aYaw, const float aRoll)
{
	myRotation.x += aPitch;
	myRotation.y += aYaw;
	myRotation.z += aRoll;
	myIsDirty = true;
}

void Transform::SetScale(const float aX, const float aY, const float aZ)
{
	myScale = { aX, aY, aZ };
	myIsDirty = true;
}

void Transform::SetScale(const CU::Vector3f aScale)
{
	myScale = aScale;
	myIsDirty = true;
}

void Transform::SetUniformScale(float aScale)
{
	myScale = { aScale, aScale, aScale };
	myIsDirty = true;
}