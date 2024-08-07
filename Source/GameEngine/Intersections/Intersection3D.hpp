#pragma once
#include <vector>
#include "Math/Vector3.hpp"
#include "Plane.hpp"
#include "Sphere.hpp"
#include "AABB3D.hpp"
#include "Ray.hpp"
#include "PlaneVolume.hpp"

namespace CommonUtilities
{
	// If the ray is parallel to the plane, aOutIntersectionPoint remains unchanged. If
	// the ray is in the plane, true is returned, if not, false is returned. If the ray
	// isn't parallel to the plane and hits, the intersection point is stored in
	// aOutIntersectionPoint and true returned otherwise false is returned and
	// outIntersectionPoint is unchanged.
	template <class T>
	bool IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay)
	{
		T tolerance = (T)0.0001;
		T d = aPlane.GetPoint().Dot(aPlane.GetNormal());
		T numerator = d - aRay.GetPoint().Dot(aPlane.GetNormal());
		T denominator = aRay.GetDirection().Dot(aPlane.GetNormal());

		// Ray is parallel to the plane
		if (denominator < tolerance && denominator > -tolerance)
		{
			// Ray Origin is on the plane
			if (numerator < tolerance && numerator > -tolerance)
			{
				return true;
			}

			return false;
		}

		T t = numerator / denominator;
		// Ray is pointing away from the plane
		if (t < 0)
		{
			return false;
		}

		return true;
	}

	template <class T>
	bool IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay, Vector3<T>& outIntersectionPoint)
	{
		T tolerance = (T)0.0001;
		T d = aPlane.GetPoint().Dot(aPlane.GetNormal());
		T numerator = d - aRay.GetPoint().Dot(aPlane.GetNormal());
		T denominator = aRay.GetDirection().Dot(aPlane.GetNormal());

		// Ray is parallel to the plane
		if (denominator < tolerance && denominator > -tolerance)
		{
			// Ray Origin is on the plane
			if (numerator < tolerance && numerator > -tolerance)
			{
				return true;
			}

			return false;
		}

		T t = numerator / denominator;

		// Ray is pointing away from the plane
		if (t < 0)
		{
			return false;
		}

		outIntersectionPoint = aRay.GetPoint() + aRay.GetDirection() * t;
		return true;
	}

	// If no collision, aOutIntersectionPoint remains unchanged.
	// If The sphere overlaps the AABB true is returned, if not, false is returned.
	// The point on the AABB closest to the sphere centre is saved in
	// aOutIntersectionPoint.
	// A sphere touching the aabb is considered overlapping.
	template <class T>
	bool IntersectionSphereAABB(const Sphere<T>& aSphere, const AABB3D<T>& aAABB3D, Vector3<T>& outIntersectionPoint = Vector3<T>())
	{
		Vector3<T> closestPoint = aSphere.GetPoint();

		if (aSphere.GetPoint().x <= aAABB3D.GetMin().x)
		{
			closestPoint.x = aAABB3D.GetMin().x;
		}
		else if (aSphere.GetPoint().x >= aAABB3D.GetMax().x)
		{
			closestPoint.x = aAABB3D.GetMax().x;
		}

		if (aSphere.GetPoint().y <= aAABB3D.GetMin().y)
		{
			closestPoint.y = aAABB3D.GetMin().y;
		}
		else if (aSphere.GetPoint().y >= aAABB3D.GetMax().y)
		{
			closestPoint.y = aAABB3D.GetMax().y;
		}

		if (aSphere.GetPoint().z <= aAABB3D.GetMin().z)
		{
			closestPoint.z = aAABB3D.GetMin().z;
		}
		else if (aSphere.GetPoint().z >= aAABB3D.GetMax().z)
		{
			closestPoint.z = aAABB3D.GetMax().z;
		}

		if (aSphere.IsInside(closestPoint))
		{
			outIntersectionPoint = closestPoint;
			return true;
		}

		return false;
	}

	// If the ray intersects the AABB, true is returned, if not, false is returned.
	// A ray in one of the AABB's sides is counted as intersecting it.
	template <class T>
	bool IntersectionAABBRay(const AABB3D<T>& aAABB, const Ray<T>& aRay)
	{
		std::vector<Plane<T>> planeVector;

		planeVector.push_back(Plane<T>(aAABB.GetMin(), Vector3<T>(-1, 0, 0)));
		planeVector.push_back(Plane<T>(aAABB.GetMin(), Vector3<T>(0, -1, 0)));
		planeVector.push_back(Plane<T>(aAABB.GetMin(), Vector3<T>(0, 0, -1)));
		planeVector.push_back(Plane<T>(aAABB.GetMax(), Vector3<T>(1, 0, 0)));
		planeVector.push_back(Plane<T>(aAABB.GetMax(), Vector3<T>(0, 1, 0)));
		planeVector.push_back(Plane<T>(aAABB.GetMax(), Vector3<T>(0, 0, 1)));

		for each (Plane<T> plane in planeVector)
		{
			Vector3<T> intersectionPoint;
			if (IntersectionPlaneRay(plane, aRay, intersectionPoint))
			{
				if (aAABB.IsInside(intersectionPoint))
				{
					return true;
				}
			}
		}

		return false;
	}

	template <class T>
	bool IntersectionAABBRay(const AABB3D<T>& aAABB, const Ray<T>& aRay, Vector3<T>& outIntersectionPoint = Vector3<T>())
	{
		std::vector<Plane<T>> planeVector;

		planeVector.push_back(Plane<T>(aAABB.GetMin(), Vector3<T>(-1, 0, 0)));
		planeVector.push_back(Plane<T>(aAABB.GetMin(), Vector3<T>(0, -1, 0)));
		planeVector.push_back(Plane<T>(aAABB.GetMin(), Vector3<T>(0, 0, -1)));
		planeVector.push_back(Plane<T>(aAABB.GetMax(), Vector3<T>(1, 0, 0)));
		planeVector.push_back(Plane<T>(aAABB.GetMax(), Vector3<T>(0, 1, 0)));
		planeVector.push_back(Plane<T>(aAABB.GetMax(), Vector3<T>(0, 0, 1)));

		Vector3<T> closestIntersectionPoint;
		float closestDistanceSqr = FLT_MAX;
		bool hit = false;

		for each (Plane<T> plane in planeVector)
		{
			Vector3<T> intersectionPoint;
			if (IntersectionPlaneRay(plane, aRay, intersectionPoint))
			{
				hit = aAABB.IsInside(intersectionPoint);
				if (hit)
				{
					float pointDistanceSqr = Vector3<T>(intersectionPoint - aRay.GetPoint()).LengthSqr();
					if (pointDistanceSqr < closestDistanceSqr)
					{
						closestDistanceSqr = pointDistanceSqr;
						closestIntersectionPoint = intersectionPoint;
					}
				}
			}
		}
		
		return hit;
	}

	// If the ray intersects the sphere, true is returned, if not, false is returned.
	// A ray intersecting the surface of the sphere is considered as intersecting it.
	template <class T>
	bool IntersectionSphereRay(const Sphere<T>& aSphere, const Ray<T>& aRay)
	{
		Vector3<T> sphereCenterToRayPoint = aSphere.GetPoint() - aRay.GetPoint();
		T distanceProjectedOntoRay = sphereCenterToRayPoint.Dot(aRay.GetDirection());

		if (distanceProjectedOntoRay < 0)
		{
			return false;
		}

		if (0 > aSphere.GetRadiusSqr() - sphereCenterToRayPoint.LengthSqr() + (distanceProjectedOntoRay * distanceProjectedOntoRay))
		{
			return false;
		}

		return true;
	}

	template<class T>
	bool IntersectionBetweenAABBS(const AABB3D<T>& aBoundingBoxOne, const AABB3D<T>& aBoundingBoxTwo)
	{
		if (aBoundingBoxOne.IsInside(aBoundingBoxTwo.GetMin())) { return true; }
		if (aBoundingBoxOne.IsInside(aBoundingBoxTwo.GetMax())) { return true; }
		if (aBoundingBoxOne.IsInside(Vector3<float>(aBoundingBoxTwo.GetMax().x, aBoundingBoxTwo.GetMax().y, aBoundingBoxTwo.GetMin().z))) { return true; }
		if (aBoundingBoxOne.IsInside(Vector3<float>(aBoundingBoxTwo.GetMax().x, aBoundingBoxTwo.GetMin().y, aBoundingBoxTwo.GetMin().z))) { return true; }
		if (aBoundingBoxOne.IsInside(Vector3<float>(aBoundingBoxTwo.GetMax().x, aBoundingBoxTwo.GetMin().y, aBoundingBoxTwo.GetMax().z))) { return true; }
		if (aBoundingBoxOne.IsInside(Vector3<float>(aBoundingBoxTwo.GetMin().x, aBoundingBoxTwo.GetMax().y, aBoundingBoxTwo.GetMin().z))) { return true; }
		if (aBoundingBoxOne.IsInside(Vector3<float>(aBoundingBoxTwo.GetMin().x, aBoundingBoxTwo.GetMin().y, aBoundingBoxTwo.GetMax().z))) { return true; }
		if (aBoundingBoxOne.IsInside(Vector3<float>(aBoundingBoxTwo.GetMin().x, aBoundingBoxTwo.GetMax().y, aBoundingBoxTwo.GetMax().z))) { return true; }

		return false;
	}

	template<class T>
	bool IntersectionBetweenSpheres(const Sphere<T>& aSphereOne, const Sphere<T>& aSphereTwo)
	{
		Vector2<T> differenceFromCenterToCenter = aSphereOne.GetPoint() - aSphereTwo.GetPoint();
		T distance = differenceFromCenterToCenter.LengthSqr();

		if (distance <= aSphereTwo.GetRadiusSqr() + aSphereOne.GetRadiusSqr())
		{
			return true;
		}

		return false;
	}

	template<class T>
	bool IntersectionBetweenPlaneVolumeAABB(const PlaneVolume<T>& aPlaneVolume, const AABB3D<T> aBoundingBox)
	{
		for (Vector3<T> corner : aBoundingBox.GetCorners())
		{
			if (aPlaneVolume.IsInside(corner))
			{
				return true;
			}
		}

		return false;
	}
}