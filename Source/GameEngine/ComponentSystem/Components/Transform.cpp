#include "Enginepch.h"

#include "Transform.h"
#include "GameEngine/Utility/SerializationUtils.hpp"

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
		child->UpdateToWorldMatrix(GetMatrix() * aToWorldMatrix, GetMatrix(true) * aToWorldMatrixNoScale);
	}
}

void Transform::UpdateChildrenToWorldMatrix()
{
	for (auto& child : myChildren)
	{
		child->UpdateToWorldMatrix(GetMatrix() * myToWorldMatrix, GetMatrix(true) * myToWorldMatrixNoScale);
	}
}

void Transform::SetIsDirty()
{
	myIsDirty = true;

	UpdateChildrenToWorldMatrix();
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

const CU::Matrix4x4f Transform::GetToWorldMatrix(bool aNoScale)
{
	if (aNoScale)
	{
		return myToWorldMatrixNoScale;
	}
	else
	{
		return myToWorldMatrix;
	}
}

void Transform::SetParent(Transform* aTransform)
{
	if (myParent)
	{
		myParent->RemoveChild(this);

		if (aTransform)
		{
			aTransform->AddChild(this);
		}

		myParent = aTransform;
	}
	else
	{
		if (aTransform)
		{
			aTransform->AddChild(this);
		}

		myParent = aTransform;
	}

	if (myParent)
	{
		myParent->UpdateChildrenToWorldMatrix();
	}
	else
	{
		myToWorldMatrix = CU::Matrix4x4f();
		myToWorldMatrixNoScale = CU::Matrix4x4f();
		UpdateChildrenToWorldMatrix();
	}
}

void Transform::SetParentInternal(Transform* aTransform)
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

	if (myParent)
	{
		myParent->UpdateChildrenToWorldMatrix();
	}
	else
	{
		myToWorldMatrix = CU::Matrix4x4f();
		myToWorldMatrixNoScale = CU::Matrix4x4f();
		UpdateChildrenToWorldMatrix();
	}
}

void Transform::AddChild(Transform* aTransform)
{
	if (!aTransform) return;

	for (int i = 0; i < myChildren.size(); i++)
	{
		if (myChildren.empty()) break;
		if (myChildren[i] == aTransform) return;
	}

	aTransform->SetParentInternal(this);
	myChildren.emplace_back(aTransform);
	UpdateChildrenToWorldMatrix();
}

void Transform::RemoveChild(Transform* aTransform)
{
	if (!aTransform) return;
	if (myChildren.empty()) return;

	for (int i = 0; i < myChildren.size(); i++)
	{
		if (myChildren[i] == aTransform)
		{
			aTransform->myParent = nullptr;
			myChildren.erase(myChildren.begin() + i);
			break;
		}
	}
}

const CU::Vector3f Transform::GetRightVector(bool aInWorldSpace)
{
	if (aInWorldSpace)
	{
		return GetWorldMatrix().GetRightVector();
	}
	else
	{
		return GetMatrix().GetRightVector();
	}
}

const CU::Vector3f Transform::GetUpVector(bool aInWorldSpace)
{
	if (aInWorldSpace)
	{
		return GetWorldMatrix().GetUpVector();
	}
	else
	{
		return GetMatrix().GetUpVector();
	}
}

const CU::Vector3f Transform::GetForwardVector(bool aInWorldSpace)
{
	if (aInWorldSpace)
	{
		return GetWorldMatrix().GetForwardVector();
	}
	else
	{
		return GetMatrix().GetForwardVector();
	}
}

const CU::Vector3f Transform::GetTranslation(bool aInWorldSpace) const
{
	if (aInWorldSpace)
	{
		return CU::ToVector3(CU::ToVector4(myPosition) * myToWorldMatrix);
	}
	else
	{
		return myPosition;
	}
}

const CU::Vector3f Transform::GetRotation(bool aInWorldSpace) const
{
	if (aInWorldSpace)
	{
		return CU::ToVector3(CU::ToVector4(myRotation) * myToWorldMatrix);
	}
	else
	{
		return myRotation;
	}
}

const CU::Vector3f Transform::GetScale(bool aInWorldSpace) const
{
	if (aInWorldSpace)
	{
		return CU::ToVector3(CU::ToVector4(myScale) * myToWorldMatrix);
	}
	else
	{
		return myScale;
	}
}

void Transform::SetTranslation(const CU::Vector3f aTranslation)
{
	myPosition = aTranslation;
	SetIsDirty();
}

void Transform::SetTranslation(const float aX, const float aY, const float aZ)
{
	myPosition = { aX, aY, aZ };
	SetIsDirty();
}

void Transform::AddTranslation(const CU::Vector3f aTranslation)
{
	myPosition += aTranslation;
	SetIsDirty();
}

void Transform::AddTranslation(const float aX, const float aY, const float aZ)
{
	myPosition += { aX, aY, aZ };
	SetIsDirty();
}

void Transform::SetRotation(const CU::Vector3f aRotationInDegrees)
{
	myRotation = aRotationInDegrees;
	SetIsDirty();
}

void Transform::SetRotation(const float aPitch, const float aYaw, const float aRoll)
{
	myRotation.x = aPitch;
	myRotation.y = aYaw;
	myRotation.z = aRoll;
	SetIsDirty();
}

void Transform::AddRotation(const CU::Vector3f aRotationInDegrees)
{
	myRotation += aRotationInDegrees;
	SetIsDirty();
}

void Transform::AddRotation(const float aPitch, const float aYaw, const float aRoll)
{
	myRotation.x += aPitch;
	myRotation.y += aYaw;
	myRotation.z += aRoll;
	SetIsDirty();
}

void Transform::SetScale(const CU::Vector3f aScale)
{
	myScale = aScale;
	SetIsDirty();
}

void Transform::SetScale(const float aX, const float aY, const float aZ)
{
	myScale = { aX, aY, aZ };
	SetIsDirty();
}

void Transform::SetUniformScale(float aScale)
{
	myScale = { aScale, aScale, aScale };
	SetIsDirty();
}

void Transform::AddScale(const CU::Vector3f aScale)
{
	myScale += aScale;
	SetIsDirty();
}

void Transform::AddScale(const float aX, const float aY, const float aZ)
{
	myScale += { aX, aY, aZ };
	SetIsDirty();
}

void Transform::AddUniformScale(float aScale)
{
	myScale += { aScale, aScale, aScale };
	SetIsDirty();
}

const bool Transform::IsScaled() const
{
	float tolerance = 0.005f;
	float scaleLength = myScale.LengthSqr();

	return (scaleLength < 1.0f - tolerance || scaleLength > 1.0f + tolerance);
}

bool Transform::Serialize(nl::json& outJsonObject)
{
	outJsonObject;
	return false;
}

bool Transform::Deserialize(nl::json& aJsonObject)
{
	CU::Vector3f pos;
	CU::Vector3f rot;
	CU::Vector3f scale = { 1.0f, 1.0f, 1.0f };

	if (aJsonObject.contains("Position"))
	{
		SetTranslation(Utility::DeserializeVector3<float>(aJsonObject["Position"]));
	};

	if (aJsonObject.contains("Rotation"))
	{
		SetRotation(Utility::DeserializeVector3<float>(aJsonObject["Rotation"]));
	}

	if (aJsonObject.contains("Scale"))
	{
		SetScale(Utility::DeserializeVector3<float>(aJsonObject["Scale"]));
	}

	return true;
}
