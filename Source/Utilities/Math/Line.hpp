#pragma once
#include "Math/Vector2.hpp"

namespace Math
{
	template <class T>
	class Line
	{
	public:
		// Default constructor: there is no line, the normal is the zero vector.
		Line();

		// Copy constructor.
		Line(const Line <T>& aLine);

		// Constructor that takes two points that define the line, the direction is aPoint1 - aPoint0.
		Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);

		// Init the line with two points, the same as the constructor above.
		void InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);

		// Init the line with a point and a direction.
		void InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection);

		// Returns whether a point is inside the line: it is inside when the point is on the line or on the side the normal is pointing away from.
		bool IsInside(const Vector2<T>& aPosition) const;

		// Returns the direction of the line.
		const Vector2<T>& GetDirection() const;

		// Returns the normal of the line, which is (-direction.y, direction.x).
		const Vector2<T> GetNormal() const;

		const Vector2<T> GetPoint() const;

	private:
		Vector2<T> myPoint;
		Vector2<T> myDirection;
	};

	template <class T>
	Line<T>::Line()
	{
		myPoint = { 0, 0 };
		myDirection = { 0, 0 };
	}

	template <class T>
	Line<T>::Line(const Line <T>& aLine)
	{
		*this = aLine;
	}

	template <class T>
	Line<T>::Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
	{
		myPoint = aPoint0;
		myDirection = aPoint1 - aPoint0;
	}

	template <class T>
	void Line<T>::InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
	{
		myPoint = aPoint0;
		myDirection = aPoint1 - aPoint0;
	}

	template <class T>
	void Line<T>::InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection)
	{
		myPoint = aPoint;
		myDirection = aDirection;
	}

	template <class T>
	bool Line<T>::IsInside(const Vector2<T>& aPosition) const
	{
		if (GetNormal().Dot(aPosition - myPoint) <= 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	template <class T>
	const Vector2<T>& Line<T>::GetDirection() const
	{
		return myDirection;
	}

	template <class T>
	const Vector2<T> Line<T>::GetNormal() const
	{
		Vector2<T> normal(myDirection.y, -myDirection.x);
		return normal.GetNormalized();
	}

	template<class T>
	inline const Vector2<T> Line<T>::GetPoint() const
	{
		return myPoint;
	}
}