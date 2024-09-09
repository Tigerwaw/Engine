#pragma once
#include "Math/Vector.hpp"

namespace CommonUtilities
{
	template <class T>
	class Ray
	{
	public:
		Ray<T>();
		Ray<T>(const Ray<T>& aRay);
		Ray<T>(const Vector3<T>& aOrigin, const Vector3<T>& aDirection);
		void InitWith2Points(const Vector3<T>& aOrigin, const Vector3<T>& aPoint);
		void InitWithOriginAndDirection(const Vector3<T>& aOrigin, const Vector3<T>& aDirection);
		Ray<T> GetRayinNewSpace(const Matrix4x4<T>& aMatrix) const;
		const Vector3<T> GetDirection() const;
		const Vector3<T> GetPoint() const;
	private:
		Vector3<T> myPoint;
		Vector3<T> myDirection;
	};

	template <class T>
	Ray<T>::Ray()
	{
		myPoint = Vector3<T>();
		myDirection = Vector3<T>();
	}

	template <class T>
	Ray<T>::Ray(const Ray<T>& aRay)
	{
		myPoint = aRay.myPoint;
		myDirection = aRay.myDirection.GetNormalized();
	}

	template <class T>
	Ray<T>::Ray(const Vector3<T>& aOrigin, const Vector3<T>& aDirection)
	{
		myPoint = aOrigin;
		myDirection = aDirection.GetNormalized();
	}

	template <class T>
	void Ray<T>::InitWith2Points(const Vector3<T>& aOrigin, const Vector3<T>& aPoint)
	{
		myPoint = aOrigin;
		myDirection = aPoint - aOrigin;
		myDirection.Normalize();
	}

	template <class T>
	void Ray<T>::InitWithOriginAndDirection(const Vector3<T>& aOrigin, const Vector3<T>& aDirection)
	{
		myPoint = aOrigin;
		myDirection = aDirection.GetNormalized();
	}

	template<class T>
	inline Ray<T> Ray<T>::GetRayinNewSpace(const Matrix4x4<T>& aMatrix) const
	{
		Vector3f newOrigin = ToVector3<T>(ToVector4<T>(myPoint, static_cast<T>(1.0)) * aMatrix);
		Vector3f newDirection = ToVector3<T>(ToVector4<T>(myDirection, 0) * aMatrix);

		return Ray<T>(newOrigin, newDirection);
	}

	template <class T>
	const Vector3<T> Ray<T>::GetPoint() const
	{
		return myPoint;
	}

	template <class T>
	const Vector3<T> Ray<T>::GetDirection() const
	{
		return myDirection;
	}
}