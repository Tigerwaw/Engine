#pragma once
#include <vector>
#include "Plane.hpp"

namespace CommonUtilities
{
	template <class T>
	class PlaneVolume
	{
	public:
		// Default constructor: empty PlaneVolume.
		PlaneVolume();

		// Constructor taking a list of Plane that makes up the PlaneVolume.
		PlaneVolume(const std::vector<Plane<T>>& aPlaneList);

		// Add a Plane to the PlaneVolume.
		void AddPlane(const Plane<T>& aPlane);

		// Returns whether a point is inside the PlaneVolume: it is inside when the point is on the
		// plane or on the side the normal is pointing away from for all the planes in the PlaneVolume.
		bool IsInside(const Vector3<T>& aPosition) const;

	private:
		std::vector<Plane<T>> myPlanes;
	};

	template <class T>
	PlaneVolume<T>::PlaneVolume()
	{
		myPlanes = std::vector<Plane<T>>();
	}

	template <class T>
	PlaneVolume<T>::PlaneVolume(const std::vector<Plane<T>>& aPlaneList)
	{
		myPlanes = aPlaneList;
	}

	template <class T>
	void PlaneVolume<T>::AddPlane(const Plane<T>& aPlane)
	{
		myPlanes.push_back(aPlane);
	}

	template <class T>
	bool PlaneVolume<T>::IsInside(const Vector3<T>& aPosition) const
	{
		for each (Plane<T> plane in myPlanes)
		{
			if (!plane.IsInside(aPosition))
			{
				return false;
			}
		}

		return true;
	}

}