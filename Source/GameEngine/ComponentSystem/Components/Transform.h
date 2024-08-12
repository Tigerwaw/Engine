#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "GameEngine/Math/Matrix4x4.hpp"
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/Math/Quaternion.hpp"

namespace CU = CommonUtilities;

class Transform : public Component
{
public:
	Transform(CU::Vector3f aPosition = { 0, 0, 0 }, CU::Vector3f aRotation = { 0, 0, 0 }, CU::Vector3f aScale = { 1, 1, 1 });

    void Start() override;
    void Update() override;

	void SetParent(Transform* aTransform);
	void AddChild(Transform* aTransform);
	void RemoveChild(Transform* aTransform);

	const CU::Matrix4x4f& GetMatrix(bool aNoScale = false);
	const CU::Matrix4x4f GetWorldMatrix(bool aNoScale = false);
	const CU::Matrix4x4f GetToWorldMatrix(bool aNoScale = false);

	const CU::Vector3f GetRightVector(bool aInWorldSpace = false);
	const CU::Vector3f GetUpVector(bool aInWorldSpace = false);
	const CU::Vector3f GetForwardVector(bool aInWorldSpace = false);
	const CU::Vector3f GetTranslation(bool aInWorldSpace = false);
	const CU::Vector3f GetRotation(bool aInWorldSpace = false);
	const CU::Vector3f GetScale(bool aInWorldSpace = false);

	void SetTranslation(const CU::Vector3f aTranslation);
	void SetTranslation(const float aX, const float aY, const float aZ);
	void AddTranslation(const CU::Vector3f aTranslation);
	void AddTranslation(const float aX, const float aY, const float aZ);

	void SetRotation(const CU::Vector3f aRotationInDegrees);
	void SetRotation(const float aPitch, const float aYaw, const float aRoll);
	void AddRotation(const CU::Vector3f aRotationInDegrees);
	void AddRotation(const float aPitch, const float aYaw, const float aRoll);

	void SetScale(const CU::Vector3f aScale);
	void SetScale(const float aX, const float aY, const float aZ);
	void SetUniformScale(float aScale);
	void AddScale(const CU::Vector3f aScale);
	void AddScale(const float aX, const float aY, const float aZ);
	void AddUniformScale(float aScale);

	const bool IsScaled() const;

	bool Serialize(nl::json& outJsonObject) override;
	bool Deserialize(nl::json& aJsonObject) override;
private:
	void UpdateToWorldMatrix(CU::Matrix4x4f aToWorldMatrix, CU::Matrix4x4f aToWorldMatrixNoScale);
	void UpdateChildrenToWorldMatrix();
	void SetIsDirty();
	void SetParentInternal(Transform* aTransform);
	CU::Matrix4x4f myCachedMatrix;
	CU::Matrix4x4f myCachedMatrixNoScale;
	bool myIsDirty = true;

	CU::Matrix4x4f myToWorldMatrix;
	CU::Matrix4x4f myToWorldMatrixNoScale;

	CU::Vector3f myPosition;
	CU::Vector3f myRotation;
	CU::Vector3f myScale;

	Transform* myParent = nullptr;
	std::vector<Transform*> myChildren;
};

