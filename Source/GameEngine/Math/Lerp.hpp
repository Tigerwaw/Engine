#pragma once
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

namespace CommonUtilities
{
	template<typename T>
	CU::Vector2<T> Lerp(CU::Vector2<T> aStartValue, CU::Vector2<T> aEndValue, const float aDeltaValue)
	{
		return CU::Vector2<T>::Lerp(aStartValue, aEndValue, aDeltaValue);
	}

	template<typename T>
	CU::Vector3<T> Lerp(CU::Vector3<T> aStartValue, CU::Vector3<T> aEndValue, const float aDeltaValue)
	{
		return CU::Vector3<T>::Lerp(aStartValue, aEndValue, aDeltaValue);
	}

	template<typename T>
	CU::Vector4<T> Lerp(CU::Vector4<T> aStartValue, CU::Vector4<T> aEndValue, const float aDeltaValue)
	{
		return CU::Vector4<T>::Lerp(aStartValue, aEndValue, aDeltaValue);
	}

	template<typename T>
	T Lerp(T aStartValue, T aEndValue, const float aDeltaValue)
	{
		return (aStartValue + aDeltaValue * (aEndValue - aStartValue));;
	}
}