#pragma once
#include "Math/Vector2.hpp"

namespace CommonUtilities
{
	template <class T>
	class Circle
	{
	public:
		// Default constructor: there is no circle, the radius is zero and the position is
		// the zero vector.
		Circle<T>();

		// Copy constructor.
		Circle<T>(const Circle<T>& aCircle);

		// Constructor that takes the center position and radius of the circle.
		Circle<T>(const Vector2<T>& aCenter, T aRadius);

		// Init the circle with a center and a radius, the same as the constructor above.
		void InitWithCenterAndRadius(const Vector2<T>& aCenter, T aRadius);

		// Returns whether a point is inside the sphere: it is inside when the point is on the
		// sphere surface or inside of the sphere.
		bool IsInside(const Vector2<T>& aPosition) const;
		const Vector2<T> GetPoint() const;
		const T GetRadiusSqr() const;
	private:
		Vector2<T> myPoint;
		T myRadiusSqr;
	};

	template <class T>
	Circle<T>::Circle()
	{
		myPoint = Vector2<T>();
		myRadiusSqr = 0;
	}

	template <class T>
	Circle<T>::Circle(const Circle<T>& aCircle)
	{
		myPoint = aCircle.myPoint;
		myRadiusSqr = aCircle.myRadiusSqr;
	}

	template <class T>
	Circle<T>::Circle(const Vector2<T>& aCenter, T aRadius)
	{
		myPoint = aCenter;
		myRadiusSqr = aRadius * aRadius;
	}

	template <class T>
	void Circle<T>::InitWithCenterAndRadius(const Vector2<T>& aCenter, T aRadius)
	{
		myPoint = aCenter;
		myRadiusSqr = aRadius * aRadius;
	}

	template <class T>
	bool Circle<T>::IsInside(const Vector2<T>& aPosition) const
	{
		Vector2<T> differenceFromPointToCenter = aPosition - myPoint;
		T distance = differenceFromPointToCenter.LengthSqr();

		if (distance <= myRadiusSqr)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	template <class T>
	const Vector2<T> Circle<T>::GetPoint() const
	{
		return myPoint;
	}

	template <class T>
	const T Circle<T>::GetRadiusSqr() const
	{
		return myRadiusSqr;
	}
}