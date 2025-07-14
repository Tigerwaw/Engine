#pragma once
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"

namespace Math
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
		const Vector3<T> GetOrigin() const;
	private:
		Vector3<T> myOrigin;
		Vector3<T> myDirection;
	};

	template <class T>
	Ray<T>::Ray()
	{
		myOrigin = Vector3<T>();
		myDirection = Vector3<T>();
	}

	template <class T>
	Ray<T>::Ray(const Ray<T>& aRay)
	{
		myOrigin = aRay.myOrigin;
		myDirection = aRay.myDirection.GetNormalized();
	}

	template <class T>
	Ray<T>::Ray(const Vector3<T>& aOrigin, const Vector3<T>& aDirection)
	{
		myOrigin = aOrigin;
		myDirection = aDirection.GetNormalized();
	}

	template <class T>
	void Ray<T>::InitWith2Points(const Vector3<T>& aOrigin, const Vector3<T>& aPoint)
	{
		myOrigin = aOrigin;
		myDirection = aPoint - aOrigin;
		myDirection.Normalize();
	}

	template <class T>
	void Ray<T>::InitWithOriginAndDirection(const Vector3<T>& aOrigin, const Vector3<T>& aDirection)
	{
		myOrigin = aOrigin;
		myDirection = aDirection.GetNormalized();
	}

	template<class T>
	inline Ray<T> Ray<T>::GetRayinNewSpace(const Matrix4x4<T>& aMatrix) const
	{
		Vector3f newOrigin = ToVector3<T>(ToVector4<T>(myOrigin, static_cast<T>(1.0)) * aMatrix);
		Vector3f newDirection = ToVector3<T>(ToVector4<T>(myDirection, 0) * aMatrix);

		return Ray<T>(newOrigin, newDirection);
	}

	template <class T>
	const Vector3<T> Ray<T>::GetOrigin() const
	{
		return myOrigin;
	}

	template <class T>
	const Vector3<T> Ray<T>::GetDirection() const
	{
		return myDirection;
	}
}