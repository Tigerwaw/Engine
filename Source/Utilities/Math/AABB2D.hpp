#pragma once
#include "Math/Vector2.hpp"

namespace Math
{
	template <class T>
	class AABB2D
	{
	public:
		// Default constructor: there is no AABB, both min and max points are the zero vector.
		AABB2D<T>();
		// Copy constructor.
		AABB2D<T>(const AABB2D<T>& aAABB2D);
		// Constructor taking the positions of the minimum and maximum corners.
		AABB2D<T>(const Vector2<T>& aMin, const Vector2<T>& aMax);
		// Init the AABB with the positions of the minimum and maximum corners, same as
		// the constructor above.
		void InitWithMinAndMax(const Vector2<T>& aMin, const Vector2<T>& aMax);
		// Returns whether a point is inside the AABB: it is inside when the point is on any
		// of the AABB's sides or inside of the AABB.
		bool IsInside(const Vector2<T>& aPosition) const;
		const Vector2<T> GetMin() const;
		const Vector2<T> GetMax() const;
	private:
		Vector2<T> minPoint;
		Vector2<T> maxPoint;
	};

	template<class T>
	AABB2D<T>::AABB2D()
	{
		minPoint = Vector2<T>();
		maxPoint = Vector2<T>();
	}

	template<class T>
	AABB2D<T>::AABB2D(const AABB2D<T>& aAABB2D)
	{
		minPoint = aAABB2D.minPoint;
		maxPoint = aAABB2D.maxPoint;
	}

	template<class T>
	AABB2D<T>::AABB2D(const Vector2<T>& aMin, const Vector2<T>& aMax)
	{
		minPoint = aMin;
		maxPoint = aMax;
	}

	template<class T>
	void AABB2D<T>::InitWithMinAndMax(const Vector2<T>& aMin, const Vector2<T>& aMax)
	{
		minPoint = aMin;
		maxPoint = aMax;
	}

	template<class T>
	bool AABB2D<T>::IsInside(const Vector2<T>& aPosition) const
	{
		if (aPosition.x < minPoint.x) { return false; }
		if (aPosition.x > maxPoint.x) { return false; }
		if (aPosition.y < minPoint.y) { return false; }
		if (aPosition.y > maxPoint.y) { return false; }

		return true;
	}

	template<class T>
	const Vector2<T> AABB2D<T>::GetMin() const
	{
		return minPoint;
	}

	template<class T>
	const Vector2<T> AABB2D<T>::GetMax() const
	{
		return maxPoint;
	}
}