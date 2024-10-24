#pragma once
#include <math.h>
#include <limits>

template<typename T = float>
constexpr T EPSILON_V = std::numeric_limits<T>::epsilon();

namespace CommonUtilities
{
	template<typename T>
	T Saturate(const T& aValue)
	{
		return static_cast<T>(std::fmin(std::fmax(aValue, T(0)), T(1)));
	}
}