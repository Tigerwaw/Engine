#pragma once
#include "Math/Vector3.hpp"

namespace CommonUtilities
{
	template <class T>
	class Sphere
	{
	public:
		// Default constructor: there is no sphere, the radius is zero and the position is
		// the zero vector.
		Sphere<T>();
		// Copy constructor.
		Sphere<T>(const Sphere<T>& aSphere);
		// Constructor that takes the center position and radius of the sphere.
		Sphere<T>(const Vector3<T>& aCenter, T aRadius);
		// Init the sphere with a center and a radius, the same as the constructor above.
		void InitWithCenterAndRadius(const Vector3<T>& aCenter, T aRadius);
		// Returns whether a point is inside the sphere: it is inside when the point is on the
		// sphere surface or inside of the sphere.
		bool IsInside(const Vector3<T>& aPosition) const;
		const Vector3<T> GetPoint() const;
		const T GetRadiusSqr() const;
	private:
		Vector3<T> myPoint;
		T myRadiusSqr;
	};

	template <class T>
	Sphere<T>::Sphere()
	{
		myPoint = Vector3<T>();
		myRadiusSqr = 0;
	}

	template <class T>
	Sphere<T>::Sphere(const Sphere<T>& aSphere)
	{
		myPoint = aSphere.myPoint;
		myRadiusSqr = aSphere.myRadiusSqr;
	}

	template <class T>
	Sphere<T>::Sphere(const Vector3<T>& aCenter, T aRadius)
	{
		myPoint = aCenter;
		myRadiusSqr = aRadius * aRadius;
	}

	template <class T>
	void Sphere<T>::InitWithCenterAndRadius(const Vector3<T>& aCenter, T aRadius)
	{
		myPoint = aCenter;
		myRadiusSqr = aRadius * aRadius;
	}

	template <class T>
	bool Sphere<T>::IsInside(const Vector3<T>& aPosition) const
	{
		Vector3<T> differenceFromPointToCenter = aPosition - myPoint;
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
	const Vector3<T> Sphere<T>::GetPoint() const
	{
		return myPoint;
	}

	template <class T>
	const T Sphere<T>::GetRadiusSqr() const
	{
		return myRadiusSqr;
	}
}