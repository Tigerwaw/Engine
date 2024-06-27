#pragma once
#include <vector>

#include "Math/Vector2.hpp"
#include "Ray2D.hpp"
#include "Line.hpp"
#include "LineVolume.hpp"
#include "Circle.hpp"
#include "AABB2D.hpp"

namespace CommonUtilities
{
	template <class T>
	struct DetailedCollisionInfo2D
	{
		Vector2<T> hitPoint = { 0, 0 };
		Vector2<T> hitNormal = { 0, 0 };
	};

	template <class T>
	bool IntersectionLineRay(const Line<T>& aLine, const Ray2D<T>& aRay)
	{
		Vector2<T> a1 = aLine.GetPoint();
		Vector2<T> a2 = aLine.GetPoint() + aLine.GetDirection();
		Vector2<T> b1 = aRay.GetPoint();
		Vector2<T> b2 = aRay.GetPoint() + aRay.GetDirection();

		if (aRay.GetDirection().Dot(aLine.GetNormal()) > 0)
		{
			return false;
		}

		if (aLine.IsInside(aRay.GetPoint()))
		{
			return false;
		}

		//T denominator = aLine.GetDirection().x * aRay.GetDirection().y - aLine.GetDirection().y * aRay.GetDirection().x;
		T denominator = (a1.x - a2.x) * (b1.y - b2.y) - (a1.y - a2.y) * (b1.x - b2.x);

		T tolerance = (T)0.0001;
		bool denominatorIsZero = denominator < tolerance && denominator > -tolerance;
		if (denominatorIsZero)
		{
			return false;
		}

		return true;
	}

	template <class T>
	bool IntersectionLineRay(const Line<T>& aLine, const Ray2D<T>& aRay, DetailedCollisionInfo2D<T>& outCollisionInfo)
	{
		Vector2<T> a1 = aLine.GetPoint();
		Vector2<T> a2 = aLine.GetPoint() + aLine.GetDirection();
		Vector2<T> b1 = aRay.GetPoint();
		Vector2<T> b2 = aRay.GetPoint() + aRay.GetDirection();

		if (aRay.GetDirection().Dot(aLine.GetNormal()) > 0)
		{
			return false;
		}

		if (aLine.IsInside(aRay.GetPoint()))
		{
			return false;
		}

		//T denominator = aLine.GetDirection().x * aRay.GetDirection().y - aLine.GetDirection().y * aRay.GetDirection().x;
		T denominator = (a1.x - a2.x) * (b1.y - b2.y) - (a1.y - a2.y) * (b1.x - b2.x);

		T tolerance = (T)0.0001;
		bool denominatorIsZero = denominator < tolerance && denominator > -tolerance;
		if (denominatorIsZero)
		{
			return false;
		}

		T xNumerator = (a1.x * a2.y - a1.y * a2.x) * (b1.x - b2.x) - (a1.x - a2.x) * (b1.x * b2.y - b1.y * b2.x);
		T yNumerator = (a1.x * a2.y - a1.y * a2.x) * (b1.y - b2.y) - (a1.y - a2.y) * (b1.x * b2.y - b1.y * b2.x);

		outCollisionInfo.hitPoint.x = xNumerator / denominator;
		outCollisionInfo.hitPoint.y = yNumerator / denominator;
		outCollisionInfo.hitNormal = aLine.GetNormal();

		return true;
	}


	template <class T>
	bool IntersectionLineVolumeRay(const LineVolume<T>& aLineVolume, const Ray2D<T>& aRay)
	{
		for each (Line<T> line in aLineVolume.GetLines())
		{
			if (IntersectionLineRay(line, aRay))
			{
				return true;
			}
		}

		return false;
	}

	template <class T>
	bool IntersectionLineVolumeRay(const LineVolume<T>& aLineVolume, const Ray2D<T>& aRay, DetailedCollisionInfo2D<T>& outCollisionInfo)
	{
		for each (Line<T> line in aLineVolume.GetLines())
		{
			if (IntersectionLineRay(line, aRay, outCollisionInfo))
			{
				return true;
			}
		}

		return false;
	}

	template<class T>
	bool IntersectionLineCircle(const Line<T>& aLine, const Circle<T>& aCircle)
	{
		T distanceAlongLineClosestToCircleCenter = aLine.GetDirection().Dot(aCircle.GetPoint() - aLine.GetPoint());
		Vector2<T> closestPointOnLineToCircleCenter = aLine.GetPoint() + aLine.GetDirection() * distanceAlongLineClosestToCircleCenter;
		Vector2<T> circleCenterToHitPoint = closestPointOnLineToCircleCenter - aCircle.GetPoint();
		if (circleCenterToHitPoint.LengthSqr() > aCircle.GetRadiusSqr())
		{
			return false;
		}

		return true;
	}

	template<class T>
	bool IntersectionLineCircle(const Line<T>& aLine, const Circle<T>& aCircle, DetailedCollisionInfo2D<T>& outCollisionInfo)
	{
		T distanceAlongLineClosestToCircleCenter = aLine.GetDirection().Dot(aCircle.GetPoint() - aLine.GetPoint());
		Vector2<T> closestPointOnLineToCircleCenter = aLine.GetPoint() + aLine.GetDirection() * distanceAlongLineClosestToCircleCenter;
		Vector2<T> circleCenterToHitPoint = closestPointOnLineToCircleCenter - aCircle.GetPoint();
		if (circleCenterToHitPoint.LengthSqr() > aCircle.GetRadiusSqr())
		{
			return false;
		}

		outCollisionInfo.hitPoint = closestPointOnLineToCircleCenter;
		outCollisionInfo.hitNormal = aLine.GetNormal();
		return true;
	}


	template<class T>
	bool IntersectionLineVolumeCircle(const LineVolume<T>& aLineVolume, const Circle<T>& aCircle)
	{
		for each (Line<T> line in aLineVolume.GetLines())
		{
			if (IntersectionLineCircle(line, aCircle))
			{
				return true;
			}
		}

		return false;
	}

	template<class T>
	bool IntersectionLineVolumeCircle(const LineVolume<T>& aLineVolume, const Circle<T>& aCircle, DetailedCollisionInfo2D<T>& outCollisionInfo)
	{
		for each (Line<T> line in aLineVolume.GetLines())
		{
			if (IntersectionLineCircle(line, aCircle, outCollisionInfo))
			{
				return true;
			}
		}

		return false;
	}


	template<class T>
	bool IntersectionLineAABB(const Line<T>& aLine, const AABB2D<T>& aAABB)
	{
		Vector2<T> cornerSW(aAABB.GetMin());
		Vector2<T> cornerSE(aAABB.GetMax().x, aAABB.GetMin().y);
		Vector2<T> cornerNE(aAABB.GetMax());
		Vector2<T> cornerNW(aAABB.GetMin().x, aAABB.GetMax().y);
		if (aLine.IsInside(cornerSW)) { return true; }
		if (aLine.IsInside(cornerSE)) { return true; }
		if (aLine.IsInside(cornerNE)) { return true; }
		if (aLine.IsInside(cornerNW)) { return true; }

		return false;
	}

	template<class T>
	bool IntersectionLineAABB(const Line<T>& aLine, const AABB2D<T>& aAABB, DetailedCollisionInfo2D<T>& outCollisionInfo)
	{
		bool returnValue = false;
		Vector2<T> hitCorner;

		Vector2<T> cornerSW(aAABB.GetMin());
		Vector2<T> cornerSE(aAABB.GetMax().x, aAABB.GetMin().y);
		Vector2<T> cornerNE(aAABB.GetMax());
		Vector2<T> cornerNW(aAABB.GetMin().x, aAABB.GetMax().y);

		if (aLine.IsInside(cornerSW)) 
		{ 
			hitCorner = cornerSW;
			returnValue = true;
		}
		if (aLine.IsInside(cornerSE))
		{ 
			hitCorner = cornerSE;
			returnValue = true;
		}
		if (aLine.IsInside(cornerNE))
		{ 
			hitCorner = cornerNE;
			returnValue = true;
		}
		if (aLine.IsInside(cornerNW))
		{ 
			hitCorner = cornerNW;
			returnValue = true;
		}

		if (returnValue)
		{
			outCollisionInfo.hitPoint = hitCorner;
			outCollisionInfo.hitNormal = aLine.GetNormal();
		}

		return returnValue;
	}


	template<class T>
	bool IntersectionLineVolumeAABB(const LineVolume<T>& aLineVolume, const AABB2D<T>& aAABB)
	{
		return false;
	}

	template<class T>
	bool IntersectionLineVolumeAABB(const LineVolume<T>& aLineVolume, const AABB2D<T>& aAABB, DetailedCollisionInfo2D<T>& outCollisionInfo)
	{
		return false;
	}


	template <class T>
	bool IntersectionAABBRay(const AABB2D<T>& aAABB, const Ray2D<T>& aRay)
	{
		Vector2<T> cornerSW(aAABB.GetMin());
		Vector2<T> cornerSE(aAABB.GetMax().x, aAABB.GetMin().y);
		Vector2<T> cornerNE(aAABB.GetMax());
		Vector2<T> cornerNW(aAABB.GetMin().x, aAABB.GetMax().y);

		LineVolume<T> lineVolume;
		lineVolume.AddLine(Line<T>(cornerSW, cornerSE));
		lineVolume.AddLine(Line<T>(cornerSE, cornerNE));
		lineVolume.AddLine(Line<T>(cornerNE, cornerNW));
		lineVolume.AddLine(Line<T>(cornerNW, cornerSW));

		for each (Line<T> line in lineVolume.GetLines())
		{
			DetailedCollisionInfo2D<T> collisionInfo;
			if (IntersectionLineRay(line, aRay, collisionInfo))
			{
				if (aAABB.IsInside(collisionInfo.hitPoint))
				{
					return true;
				}
			}
		}

		return false;
	}

	template <class T>
	bool IntersectionAABBRay(const AABB2D<T>& aAABB, const Ray2D<T>& aRay, DetailedCollisionInfo2D<T>& outCollisionInfo)
	{
		Vector2<T> cornerSW(aAABB.GetMin());
		Vector2<T> cornerSE(aAABB.GetMax().x, aAABB.GetMin().y);
		Vector2<T> cornerNE(aAABB.GetMax());
		Vector2<T> cornerNW(aAABB.GetMin().x, aAABB.GetMax().y);

		LineVolume<T> lineVolume;
		lineVolume.AddLine(Line<T>(cornerSW, cornerSE));
		lineVolume.AddLine(Line<T>(cornerSE, cornerNE));
		lineVolume.AddLine(Line<T>(cornerNE, cornerNW));
		lineVolume.AddLine(Line<T>(cornerNW, cornerSW));

		for each (Line<T> line in lineVolume.GetLines())
		{
			if (IntersectionLineRay(line, aRay, outCollisionInfo))
			{
				if (aAABB.IsInside(outCollisionInfo.hitPoint))
				{
					return true;
				}
			}
		}

		return false;
	}


	template <class T>
	bool IntersectionCircleAABB(const Circle<T>& aCircle, const AABB2D<T>& aAABB2D)
	{
		Vector2<T> closestPoint = aCircle.GetPoint();

		closestPoint.x = Clamp(aCircle.GetPoint().x, aAABB2D.GetMin().x, aAABB2D.GetMax().x);
		closestPoint.y = Clamp(aCircle.GetPoint().y, aAABB2D.GetMin().y, aAABB2D.GetMax().y);

		if (aCircle.IsInside(closestPoint))
		{
			return true;
		}

		return false;
	}

	template <class T>
	bool IntersectionCircleAABB(const Circle<T>& aCircle, const AABB2D<T>& aAABB2D, DetailedCollisionInfo2D<T>& outCollisionInfo)
	{
		Vector2<T> closestPoint = aCircle.GetPoint();

		closestPoint.x = Clamp(aCircle.GetPoint().x, aAABB2D.GetMin().x, aAABB2D.GetMax().x);
		closestPoint.y = Clamp(aCircle.GetPoint().y, aAABB2D.GetMin().y, aAABB2D.GetMax().y);

		if (aCircle.IsInside(closestPoint))
		{
			outCollisionInfo.hitPoint = closestPoint;
			outCollisionInfo.hitNormal = Vector2<T>(aCircle.GetPoint() - closestPoint).GetNormalized();
			return true;
		}

		return false;
	}


	template <class T>
	bool IntersectionCircleRay(const Circle<T>& aCircle, const Ray2D<T>& aRay)
	{
		T distanceAlongRayClosestToCircleCenter = aRay.GetDirection().Dot(aCircle.GetPoint() - aRay.GetPoint());

		if (distanceAlongRayClosestToCircleCenter < 0)
		{
			return false;
		}

		Vector2<T> closestPointOnRayToCircleCenter = aRay.GetPoint() + aRay.GetDirection() * distanceAlongRayClosestToCircleCenter;

		Vector2<T> circleCenterToHitPoint = closestPointOnRayToCircleCenter - aCircle.GetPoint();
		if (circleCenterToHitPoint.LengthSqr() > aCircle.GetRadiusSqr())
		{
			return false;
		}

		return true;
	}

	template <class T>
	bool IntersectionCircleRay(const Circle<T>& aCircle, const Ray2D<T>& aRay, DetailedCollisionInfo2D<T>& outCollisionInfo)
	{
		T distanceAlongRayClosestToCircleCenter = aRay.GetDirection().Dot(aCircle.GetPoint() - aRay.GetPoint());

		if (distanceAlongRayClosestToCircleCenter < 0)
		{
			return false;
		}

		Vector2<T> closestPointOnRayToCircleCenter = aRay.GetPoint() + aRay.GetDirection() * distanceAlongRayClosestToCircleCenter;

		Vector2<T> circleCenterToHitPoint = closestPointOnRayToCircleCenter - aCircle.GetPoint();
		if (circleCenterToHitPoint.LengthSqr() > aCircle.GetRadiusSqr())
		{
			return false;
		}

		outCollisionInfo.hitPoint = closestPointOnRayToCircleCenter;
		outCollisionInfo.hitNormal = circleCenterToHitPoint.GetNormalized();

		return true;
	}


	template<class T>
	bool IntersectionBetweenAABBs(const AABB2D<T>& aAABB, const AABB2D<T>& anotherAABB)
	{
		if (aAABB.GetMin().x > anotherAABB.GetMax().x) { return false; }
		if (aAABB.GetMax().x < anotherAABB.GetMin().x) { return false; }
		if (aAABB.GetMin().y > anotherAABB.GetMax().y) { return false; }
		if (aAABB.GetMax().y < anotherAABB.GetMin().y) { return false; }

		return true;
	}

	template<class T>
	bool IntersectionBetweenAABBs(const AABB2D<T>& aAABB, const AABB2D<T>& anotherAABB, DetailedCollisionInfo2D<T>& outCollisionInfo)
	{
		if (aAABB.GetMin().x > anotherAABB.GetMax().x) { return false; }
		if (aAABB.GetMax().x < anotherAABB.GetMin().x) { return false; }
		if (aAABB.GetMin().y > anotherAABB.GetMax().y) { return false; }
		if (aAABB.GetMax().y < anotherAABB.GetMin().y) { return false; }

		return true;
	}


	template<class T>
	bool IntersectionBetweenCircles(const Circle<T>& aCircle, const Circle<T>& anotherCircle)
	{
		Vector2<T> differenceFromCenterToCenter = aCircle.GetPoint() - anotherCircle.GetPoint();
		T distance = differenceFromCenterToCenter.LengthSqr();

		if (distance <= anotherCircle.GetRadiusSqr() + aCircle.GetRadiusSqr())
		{
			return true;
		}
		
		return false;
	}

	template<class T>
	bool IntersectionBetweenCircles(const Circle<T>& aCircle, const Circle<T>& anotherCircle, DetailedCollisionInfo2D<T>& outCollisionInfo)
	{
		Vector2<T> differenceFromCenterToCenter = aCircle.GetPoint() - anotherCircle.GetPoint();
		T distanceSquared = differenceFromCenterToCenter.LengthSqr();

		if (distanceSquared <= anotherCircle.GetRadiusSqr() + aCircle.GetRadiusSqr())
		{
			outCollisionInfo.hitNormal = differenceFromCenterToCenter.GetNormalized();
			outCollisionInfo.hitPoint = anotherCircle.GetPoint() + outCollisionInfo.hitNormal * sqrtf(anotherCircle.GetRadiusSqr());
			return true;
		}

		return false;
	}


	template<class T>
	bool IntersectionBetweenRays(const Ray2D<T>& aRay, const Ray2D<T>& anotherRay)
	{
		return false;
	}

	template<class T>
	bool IntersectionBetweenRays(const Ray2D<T>& aRay, const Ray2D<T>& anotherRay, DetailedCollisionInfo2D<T>& outCollisionInfo)
	{
		return false;
	}

	inline float Clamp(const float& aValue, const float& aMinValue, const float& aMaxValue)
	{
		float newValue = 0;

		if (aValue <= aMinValue)
		{
			newValue = aMinValue;
		}
		else if (aValue >= aMaxValue)
		{
			newValue = aMaxValue;
		}

		return newValue;
	}
}
