#pragma once
#include <vector>
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/Math/Matrix.hpp"

namespace CommonUtilities
{
	template <class T>
	class AABB3D
	{
	public:
		// Default constructor: there is no AABB, both min and max points are the zero vector.
		AABB3D<T>();
		// Copy constructor.
		AABB3D<T>(const AABB3D<T>& aAABB3D);
		// Constructor taking the positions of the minimum and maximum corners.
		AABB3D<T>(const Vector3<T>& aMin, const Vector3<T>& aMax);
		// Init the AABB with the positions of the minimum and maximum corners, same as
		// the constructor above.
		void InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax);
		// Returns whether a point is inside the AABB: it is inside when the point is on any
		// of the AABB's sides or inside of the AABB.
		bool IsInside(const Vector3<T>& aPosition) const;
		const Vector3<T> GetMin() const;
		const Vector3<T> GetMax() const;
		const std::vector<Vector3<T>> GetCorners() const;
	private:
		Vector3<T> minPoint;
		Vector3<T> maxPoint;
	};

	template<class T>
	AABB3D<T>::AABB3D()
	{
		minPoint = Vector3<T>();
		maxPoint = Vector3<T>();
	}

	template<class T>
	AABB3D<T>::AABB3D(const AABB3D<T>& aAABB3D)
	{
		minPoint = aAABB3D.minPoint;
		maxPoint = aAABB3D.maxPoint;
	}

	template<class T>
	AABB3D<T>::AABB3D(const Vector3<T>& aMin, const Vector3<T>& aMax)
	{
		minPoint = aMin;
		maxPoint = aMax;
	}

	template<class T>
	void AABB3D<T>::InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax)
	{
		minPoint = aMin;
		maxPoint = aMax;
	}

	template<class T>
	bool AABB3D<T>::IsInside(const Vector3<T>& aPosition) const
	{
		if (aPosition.x < minPoint.x) { return false; }
		if (aPosition.x > maxPoint.x) { return false; }
		if (aPosition.y < minPoint.y) { return false; }
		if (aPosition.y > maxPoint.y) { return false; }
		if (aPosition.z < minPoint.z) { return false; }
		if (aPosition.z > maxPoint.z) { return false; }

		return true;
	}

	template<class T>
	const AABB3D<T> operator*(const AABB3D<T>& aAABB3D, const Matrix4x4<T>& aMatrix)
	{
		Vector3f newMinPoint = ToVector3(ToVector4(aAABB3D.GetMin(), 1.0f) * aMatrix);
		Vector3f newMaxPoint = ToVector3(ToVector4(aAABB3D.GetMax(), 1.0f) * aMatrix);
		return AABB3D<T>(newMinPoint, newMaxPoint);
	}

	template<class T>
	const Vector3<T> AABB3D<T>::GetMin() const
	{
		return minPoint;
	}

	template<class T>
	const Vector3<T> AABB3D<T>::GetMax() const
	{
		return maxPoint;
	}

	template<class T>
	inline const std::vector<Vector3<T>> AABB3D<T>::GetCorners() const
	{
		std::vector<Vector3<T>> corners;
		corners.emplace_back(minPoint);
		corners.emplace_back(Vector3<T>(maxPoint.x, minPoint.y, minPoint.z));
		corners.emplace_back(Vector3<T>(maxPoint.x, minPoint.y, maxPoint.z));
		corners.emplace_back(Vector3<T>(minPoint.x, minPoint.y, maxPoint.z));

		corners.emplace_back(Vector3<T>(minPoint.x, maxPoint.y, minPoint.z));
		corners.emplace_back(Vector3<T>(maxPoint.x, maxPoint.y, minPoint.z));
		corners.emplace_back(maxPoint);
		corners.emplace_back(Vector3<T>(minPoint.x, maxPoint.y, maxPoint.z));
		return corners;
	}
}