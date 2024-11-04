#pragma once
#include "Math/Vector.hpp"
#include "Intersections/LineSegment3D.hpp"
#include "Intersections/Plane.hpp"
#include <array>
#include <algorithm>

namespace CommonUtilities
{
	template <class T>
	class Triangle
	{
	public:
		Triangle<T>(Vector3<T> aPointOne, Vector3<T> aPointTwo, Vector3<T> aPointThree);
		const std::array<Vector3<T>, 3>& GetPoints() const;

		Vector3<T> ClosestPointOnTriangle(Vector3<T> aPoint);
	private:
		std::array<Vector3<T>, 3> points;
	};

	template<class T>
	inline Triangle<T>::Triangle(Vector3<T> aPointOne, Vector3<T> aPointTwo, Vector3<T> aPointThree)
	{
		points[0] = aPointOne;
		points[1] = aPointTwo;
		points[2] = aPointThree;
	}

	template<class T>
	inline const std::array<Vector3<T>, 3>& Triangle<T>::GetPoints() const
	{
		return points;
	}

	template<class T>
	inline Vector3<T> Triangle<T>::ClosestPointOnTriangle(Vector3<T> aPoint)
	{
		Plane<T> plane(points[0], points[1], points[2]);
		Vector3<T> closestPoint = plane.ClosestPointOnPlane(aPoint);

		LineSegment3D<T> AB(points[0], points[1]);
		LineSegment3D<T> BC(points[1], points[2]);
		LineSegment3D<T> CA(points[2], points[0]);

		Vector3<T> c1 = AB.ClosestPointOnLine(aPoint);
		Vector3<T> c2 = BC.ClosestPointOnLine(aPoint);
		Vector3<T> c3 = CA.ClosestPointOnLine(aPoint);

		T mag1 = (aPoint - c1).LengthSqr();
		T mag2 = (aPoint - c2).LengthSqr();
		T mag3 = (aPoint - c3).LengthSqr();

		T min = (T)std::fmin(mag1, std::fmin(mag2, mag3));

		if (min == mag1) return c1;
		else if (min == mag2) return c2;
		else return c3;
	}
}