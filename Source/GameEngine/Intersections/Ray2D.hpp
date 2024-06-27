#pragma once
#include "Math/Vector2.hpp"

namespace CommonUtilities
{
	template <class T>
	class Ray2D
	{
	public:
		Ray2D<T>();
		Ray2D<T>(const Ray2D<T>& aRay);
		Ray2D<T>(const Vector2<T>& aOrigin, const Vector2<T>& aDirection);
		void InitWith2Points(const Vector2<T>& aOrigin, const Vector2<T>& aPoint);
		void InitWithOriginAndDirection(const Vector2<T>& aOrigin, const Vector2<T>& aDirection);
		const Vector2<T> GetDirection() const;
		const Vector2<T> GetPoint() const;
	private:
		Vector2<T> myPoint;
		Vector2<T> myDirection;
	};

	template <class T>
	Ray2D<T>::Ray2D()
	{
		myPoint = Vector2<T>();
		myDirection = Vector2<T>().GetNormalized();
	}

	template <class T>
	Ray2D<T>::Ray2D(const Ray2D<T>& aRay)
	{
		myPoint = aRay.myPoint;
		myDirection = aRay.myDirection.GetNormalized();
	}

	template <class T>
	Ray2D<T>::Ray2D(const Vector2<T>& aOrigin, const Vector2<T>& aDirection)
	{
		myPoint = aOrigin;
		myDirection = aDirection.GetNormalized();
	}

	template <class T>
	void Ray2D<T>::InitWith2Points(const Vector2<T>& aOrigin, const Vector2<T>& aPoint)
	{
		myPoint = aOrigin;
		myDirection = aPoint - aOrigin;
		myDirection.Normalize();
	}

	template <class T>
	void Ray2D<T>::InitWithOriginAndDirection(const Vector2<T>& aOrigin, const Vector2<T>& aDirection)
	{
		myPoint = aOrigin;
		myDirection = aDirection.GetNormalized();
	}

	template <class T>
	const Vector2<T> Ray2D<T>::GetPoint() const
	{
		return myPoint;
	}

	template <class T>
	const Vector2<T> Ray2D<T>::GetDirection() const
	{
		return myDirection;
	}
}

