#pragma once
#include "Math/Vector.hpp"

namespace Math
{
	template <class T>
	class Plane
	{
	public:
		// Default constructor.
		Plane();

		// Constructor taking three points where the normal is (aPoint1 - aPoint0) x (aPoint2 - aPoint0).
		Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2);

		// Constructor taking a point and a normal.
		Plane(const Vector3<T>& aPoint, const Vector3<T>& aNormal);

		// Init the plane with three points, the same as the constructor above.
		void InitWith3Points(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2);

		// Init the plane with a point and a normal, the same as the constructor above.
		void InitWithPointAndNormal(const Vector3<T>& aPoint, const Vector3<T>& aNormal);

		// Returns whether a point is inside the plane: it is inside when the point is on the plane or on the side the normal is pointing away from.
		bool IsInside(const Vector3<T>& aPosition) const;

		const Vector3<T> ClosestPointOnPlane(const Vector3<T>& aPosition) const;

		const Vector3<T>& GetPoint() const;
		const Vector3<T>& GetNormal() const;
	private:
		Vector3<T> myPoint;
		Vector3<T> myNormal;
	};

	template <class T>
	Plane<T>::Plane()
	{
		myPoint = { 0, 0, 0 };
		myNormal = { 0, 0, 0 };
	}

	template <class T>
	Plane<T>::Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
	{
		myPoint = aPoint0;
		Vector3<T> e1 = aPoint1 - aPoint0;
		Vector3<T> e3 = aPoint2 - aPoint1;
		myNormal = e1.Cross(e3);
		myNormal.Normalize();
	}

	template <class T>
	Plane<T>::Plane(const Vector3<T>& aPoint, const Vector3<T>& aNormal)
	{
		myPoint = aPoint;
		myNormal = aNormal.GetNormalized();
	}

	template <class T>
	void Plane<T>::InitWith3Points(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
	{
		myPoint = aPoint0;
		Vector3<T> e1 = aPoint1 - aPoint0;
		Vector3<T> e3 = aPoint2 - aPoint1;
		myNormal = e1.Cross(e3);
		myNormal.Normalize();
	}

	template <class T>
	void Plane<T>::InitWithPointAndNormal(const Vector3<T>& aPoint, const Vector3<T>& aNormal)
	{
		myPoint = aPoint;
		myNormal = aNormal.GetNormalized();
	}

	template <class T>
	bool Plane<T>::IsInside(const Vector3<T>& aPosition) const
	{
		return GetNormal().Dot(aPosition) - GetNormal().Dot(myPoint) <= 0;
	}

	template<class T>
	inline const Vector3<T> Plane<T>::ClosestPointOnPlane(const Vector3<T>& aPosition) const
	{
		T distance = GetNormal().Dot(aPosition) - GetPoint().Length();
		return aPosition - distance * GetNormal();
	}

	template <class T>
	const Vector3<T>& Plane<T>::GetPoint() const
	{
		return myPoint;
	}

	template <class T>
	const Vector3<T>& Plane<T>::GetNormal() const
	{
		return myNormal;
	}
}
