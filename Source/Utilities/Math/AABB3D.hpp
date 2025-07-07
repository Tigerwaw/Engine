#pragma once
#include <vector>
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"

namespace Math
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
		AABB3D<T>(const Vector3<T>& aCenter, const T aWidth, const T aHeight, const T aDepth);
		// Init the AABB with the positions of the minimum and maximum corners, same as
		// the constructor above.
		void InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax);
		void InitWithCenterAndExtents(const Vector3<T>& aCenter, const T aWidth, const T aHeight, const T aDepth);
		void InitWithCenterAndExtents(const Vector3<T>& aCenter, const Vector3<T>& aExtents);
		AABB3D<T> GetAABBinNewSpace(const Matrix4x4<T>& aMatrix) const;

		// Returns whether a point is inside the AABB: it is inside when the point is on any
		// of the AABB's sides or inside of the AABB.
		bool IsInside(const Vector3<T>& aPosition) const;
		const Vector3<T> GetMin() const;
		const Vector3<T> GetMax() const;
		const std::array<Math::Vector3<T>, 8> GetCorners() const;
		const Vector3<T> GetCenter() const;
		const Vector3<T> GetExtents() const;
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
		InitWithMinAndMax(aAABB3D.minPoint, aAABB3D.maxPoint);
	}

	template<class T>
	AABB3D<T>::AABB3D(const Vector3<T>& aMin, const Vector3<T>& aMax)
	{
		InitWithMinAndMax(aMin, aMax);
	}

	template<class T>
	inline AABB3D<T>::AABB3D(const Vector3<T>& aCenter, const T aWidth, const T aHeight, const T aDepth)
	{
		InitWithCenterAndExtents(aCenter, aWidth, aHeight, aDepth);
	}

	template<class T>
	void AABB3D<T>::InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax)
	{
		minPoint = aMin;
		maxPoint = aMax;
	}

	template<class T>
	inline void AABB3D<T>::InitWithCenterAndExtents(const Vector3<T>& aCenter, const T aWidth, const T aHeight, const T aDepth)
	{
		InitWithCenterAndExtents(aCenter, { aWidth, aHeight, aDepth });
	}

	template<class T>
	inline void AABB3D<T>::InitWithCenterAndExtents(const Vector3<T>& aCenter, const Vector3<T>& aExtents)
	{
		Vector3<T> halfExtents = aExtents * 0.5f;
		Vector3<T> newMin = aCenter - halfExtents;
		Vector3<T> newMax = aCenter + halfExtents;
		InitWithMinAndMax(newMin, newMax);
	}

	template<class T>
	inline AABB3D<T> AABB3D<T>::GetAABBinNewSpace(const Matrix4x4<T>& aMatrix) const
	{
		Vector3<T> newMin = { FLT_MAX, FLT_MAX, FLT_MAX };
		Vector3<T> newMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

		for (Vector3<T> corner : GetCorners())
		{
			corner = ToVector3(ToVector4(corner, 1.0f) * aMatrix);

			newMin.x = std::fminf(corner.x, newMin.x);
			newMax.x = std::fmaxf(corner.x, newMax.x);
			newMin.y = std::fminf(corner.y, newMin.y);
			newMax.y = std::fmaxf(corner.y, newMax.y);
			newMin.z = std::fminf(corner.z, newMin.z);
			newMax.z = std::fmaxf(corner.z, newMax.z);
		}

		AABB3D<T> newAABB(newMin, newMax);

		return newAABB;
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
	inline const std::array<Math::Vector3<T>, 8> AABB3D<T>::GetCorners() const
	{
		std::array<Math::Vector3<T>, 8> corners;
		corners[0] = minPoint;
		corners[1] = Vector3<T>(maxPoint.x, minPoint.y, minPoint.z);
		corners[2] = Vector3<T>(maxPoint.x, minPoint.y, maxPoint.z);
		corners[3] = Vector3<T>(minPoint.x, minPoint.y, maxPoint.z);

		corners[4] = Vector3<T>(minPoint.x, maxPoint.y, minPoint.z);
		corners[5] = Vector3<T>(maxPoint.x, maxPoint.y, minPoint.z);
		corners[6] = maxPoint;
		corners[7] = Vector3<T>(minPoint.x, maxPoint.y, maxPoint.z);
		return corners;
	}

	template<class T>
	inline const Vector3<T> AABB3D<T>::GetCenter() const
	{
		return Vector3<T>::Abs(maxPoint) - Vector3<T>::Abs(minPoint);
	}

	template<class T>
	inline const Vector3<T> AABB3D<T>::GetExtents() const
	{
		return Vector3<T>::Abs(maxPoint) - GetCenter();
	}
}