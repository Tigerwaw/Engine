#pragma once
#include <vector>
#include "Line.hpp"

namespace Math
{
	template <class T>
	class LineVolume
	{
	public:
		LineVolume();
		LineVolume(const std::vector<Line<T>>& aLineList);
		const std::vector<Line<T>>& GetLines() const;
		void AddLine(const Line<T>& aLine);
		const bool IsInside(const Vector2<T>& aPosition) const;

	private:
		std::vector<Line<T>> myLines;
	};

	template <class T>
	LineVolume<T>::LineVolume()
	{
		myLines = std::vector<Line<T>>();
	}

	template <class T>
	LineVolume<T>::LineVolume(const std::vector<Line<T>>& aLineList)
	{
		myLines = aLineList;
	}

	template<class T>
	const std::vector<Line<T>>& LineVolume<T>::GetLines() const
	{
		return myLines;
	}

	template <class T>
	void LineVolume<T>::AddLine(const Line<T>& aLine)
	{
		myLines.push_back(aLine);
	}

	template <class T>
	const bool LineVolume<T>::IsInside(const Vector2<T>& aPosition) const
	{
		for each (Line<T> line in myLines)
		{
			if (!line.IsInside(aPosition))
			{
				return false;
			}
		}

		return true;
	}
}