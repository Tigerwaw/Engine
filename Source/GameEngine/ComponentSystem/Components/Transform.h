#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "GameEngine/Math/Matrix4x4.hpp"
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/Math/Quaternion.hpp"

#include <vector>

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

	const CU::Vector3f GetRightVector();
	const CU::Vector3f GetUpVector();
	const CU::Vector3f GetForwardVector();
	const CU::Vector3f& GetTranslation() const { return myPosition; }
	const CU::Vector3f& GetRotation() const { return myRotation; }
	const CU::Vector3f& GetScale() const { return myScale; }

	void SetTranslation(const CU::Vector3f aTranslation);
	void SetTranslation(const float aX, const float aY, const float aZ);

	void SetRotation(const CU::Vector3f aRotationInDegrees);
	void SetRotation(const float aPitch, const float aYaw, const float aRoll);
	void AddRotation(const CU::Vector3f aRotationInDegrees);
	void AddRotation(const float aPitch, const float aYaw, const float aRoll);

	void SetScale(const float aX, const float aY, const float aZ);
	void SetScale(const CU::Vector3f aScale);
	void SetUniformScale(float aScale);
private:
	void UpdateToWorldMatrix(CU::Matrix4x4f aToWorldMatrix, CU::Matrix4x4f aToWorldMatrixNoScale);
	void SetIsDirty();
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

