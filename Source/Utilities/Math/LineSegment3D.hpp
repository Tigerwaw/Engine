#pragma once
#include "Math/Vector.hpp"
#include <algorithm>

namespace Math
{
	template <class T>
	class LineSegment3D
	{
	public:
		LineSegment3D<T>(Vector3<T> aPointOne, Vector3<T> aPointTwo);

		Vector3<T> ToVector();
		T Length();
		T LengthSqr();

		Vector3<T> ClosestPointOnLine(Vector3<T> aPoint);
	private:
		Vector3<T> pointOne;
		Vector3<T> pointTwo;
	};

	template<class T>
	inline LineSegment3D<T>::LineSegment3D(Vector3<T> aPointOne, Vector3<T> aPointTwo)
	{
		pointOne = aPointOne;
		pointTwo = aPointTwo;
	}

	template<class T>
	inline Vector3<T> LineSegment3D<T>::ToVector()
	{
		return pointTwo - pointOne;
	}

	template<class T>
	inline T LineSegment3D<T>::Length()
	{
		return ToVector().Length();
	}

	template<class T>
	inline T LineSegment3D<T>::LengthSqr()
	{
		return ToVector().LengthSqr();
	}

	template<class T>
	inline Vector3<T> LineSegment3D<T>::ClosestPointOnLine(Vector3<T> aPoint)
	{
		Vector3<T> toVector = ToVector();
		float t = (aPoint - pointOne).Dot(toVector) / toVector.Dot(toVector);
		t = std::clamp(t, (T)0, (T)1);

		Vector3<T> point = pointOne + t * ToVector();
		return point;
	}
}