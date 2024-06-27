#pragma once
#include "GameEngine/Math/Matrix4x4.hpp"
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/Math/Quaternion.hpp"

namespace CommonUtilities
{
	template <class T>
	class Transform
	{
	public:
		Transform<T>(Vector3<T> aPosition = { 0, 0, 0 }, Vector3<T> aRotation = { 0, 0, 0 }, Vector3<T> aScale = { 1, 1, 1 });

		// Copy Constructor.
		Transform<T>(const Transform<T>& aTransform);

		void operator=(const Transform<T>& aTransform);

		void MoveIntoParentSpace(Transform<T> aParentTransform);
		void MoveIntoChildSpace(Transform<T> aChildTransform);

		Vector3<T> GetRightVector();
		Vector3<T> GetUpVector();
		Vector3<T> GetForwardVector();
		Vector3<T> GetTranslation() { return myPosition; }
		Vector3<T> GetRotation() { return myRotation; }
		Vector3<T> GetScale() { return myScale; }

		void SetTranslation(const Vector3<T> aTranslation);
		void SetTranslation(const T aX, const T aY, const T aZ);

		void SetRotation(const Vector3<T> aRotationInDegrees);
		void SetRotation(const T aPitch, const T aYaw, const T aRoll);
		void AddRotation(const Vector3<T> aRotationInDegrees);
		void AddRotation(const T aPitch, const T aYaw, const T aRoll);

		void SetScale(const T aX, const T aY, const T aZ);
		void SetScale(const Vector3<T> aScale);
		void SetUniformScale(T aScale);

		Matrix4x4<T> GetMatrix(bool aNoScale = false);

	private:
		Matrix4x4<T> myCachedMatrix;
		Matrix4x4<T> myCachedMatrixNoScale;
		Vector3<T> myPosition;
		Vector3<T> myRotation;
		Vector3<T> myScale;

		bool myIsDirty = true;
	};

	template<class T>
	inline Transform<T>::Transform(Vector3<T> aPosition, Vector3<T> aRotation, Vector3<T> aScale)
	{
		myPosition = aPosition;
		myRotation = aRotation;
		myScale = aScale;
	}

	template<class T>
	Transform<T>::Transform(const Transform<T>& aTransform)
	{
		*this = aTransform;
	}

	template <class T>
	void Transform<T>::operator=(const Transform<T>& aTransform)
	{
		myPosition = aTransform.myPosition;
		myRotation = aTransform.myRotation;
		myScale = aTransform.myScale;
		myIsDirty = true;
	}

	template<class T>
	void Transform<T>::MoveIntoParentSpace(Transform<T> aParentTransform)
	{
		GetMatrix() *= aParentTransform.GetMatrix();
	}

	template<class T>
	void Transform<T>::MoveIntoChildSpace(Transform<T> aChildTransform)
	{
		GetMatrix() *= aChildTransform.GetMatrix().GetFastInverse();
	}

	template<class T>
	Vector3<T> Transform<T>::GetRightVector()
	{
		return Vector3<T>(GetMatrix()(1, 1), GetMatrix()(1, 2), GetMatrix()(1, 3));
	}

	template<class T>
	Vector3<T> Transform<T>::GetUpVector()
	{
		return Vector3<T>(GetMatrix()(2, 1), GetMatrix()(2, 2), GetMatrix()(2, 3));
	}

	template<class T>
	Vector3<T> Transform<T>::GetForwardVector()
	{
		return Vector3<T>(GetMatrix()(3, 1), GetMatrix()(3, 2), GetMatrix()(3, 3));
	}

	template<class T>
	inline void Transform<T>::SetTranslation(const Vector3<T> aTranslation)
	{
		myPosition = aTranslation;
		myIsDirty = true;
	}

	template<class T>
	inline void Transform<T>::SetTranslation(const T aX, const T aY, const T aZ)
	{
		myPosition = Vector3<T>(aX, aY, aZ);
		myIsDirty = true;
	}

	template<class T>
	inline void Transform<T>::SetRotation(const Vector3<T> aPitchYawRollVector)
	{
		myRotation = aPitchYawRollVector;
		myIsDirty = true;
	}

	template<class T>
	inline void Transform<T>::SetRotation(const T aPitch, const T aYaw, const T aRoll)
	{
		myRotation.x = aPitch;
		myRotation.y = aYaw;
		myRotation.z = aRoll;
		myIsDirty = true;
	}

	template<class T>
	inline void Transform<T>::AddRotation(const Vector3<T> aPitchYawRollVector)
	{
		myRotation += aPitchYawRollVector;
		myIsDirty = true;
	}

	template<class T>
	inline void Transform<T>::AddRotation(const T aPitch, const T aYaw, const T aRoll)
	{
		myRotation.x += aPitch;
		myRotation.y += aYaw;
		myRotation.z += aRoll;
		myIsDirty = true;
	}

	template<class T>
	inline void Transform<T>::SetScale(const T aX, const T aY, const T aZ)
	{
		myScale = { aX, aY, aZ };
		myIsDirty = true;
	}

	template<class T>
	inline void Transform<T>::SetScale(const Vector3<T> aScale)
	{
		myScale = aScale;
		myIsDirty = true;
	}

	template<class T>
	inline void Transform<T>::SetUniformScale(T aScale)
	{
		myScale = { aScale, aScale, aScale };
		myIsDirty = true;
	}

	template<class T>
	inline Matrix4x4<T> Transform<T>::GetMatrix(bool aNoScale)
	{
		if (myIsDirty)
		{
			myCachedMatrix = Matrix4x4<T>();
			myCachedMatrixNoScale = Matrix4x4<T>();

			Matrix4x4<T> scaleMatrix;
			scaleMatrix(1, 1) = myScale.x;
			scaleMatrix(2, 2) = myScale.y;
			scaleMatrix(3, 3) = myScale.z;

			myCachedMatrix *= scaleMatrix;

			Matrix4x4<T> rotationMatrix = Matrix4x4<T>::CreateRollPitchYawMatrix(myRotation);
			myCachedMatrix *= rotationMatrix;
			myCachedMatrixNoScale *= rotationMatrix;

			Matrix4x4<T> translationMatrix;
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
}