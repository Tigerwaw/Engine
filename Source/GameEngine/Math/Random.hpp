#pragma once
#include <random>

inline thread_local std::mt19937_64 dre(std::random_device{}());

inline float Random()
{
	std::uniform_real_distribution<float> uid(0.0f, 1.0f);
	return uid(dre);
}

inline float Random(float aMin, float aMax)
{
	std::uniform_real_distribution<float> uid(aMin, aMax);
	return uid(dre);
}

inline int Random(int aMin, int aMax)
{
	std::uniform_int_distribution<int> uid(aMin, aMax);
	return uid(dre);
}

inline CU::Vector2f Random(CU::Vector2f aMin, CU::Vector2f aMax)
{
	return CU::Vector2f(Random(aMin.x, aMax.x), Random(aMin.y, aMax.y));
}

inline CU::Vector3f Random(CU::Vector3f aMin, CU::Vector3f aMax)
{
	return CU::Vector3f(Random(aMin.x, aMax.x), Random(aMin.y, aMax.y), Random(aMin.z, aMax.z));
}

inline CU::Vector4f Random(CU::Vector4f aMin, CU::Vector4f aMax)
{
	return CU::Vector4f(Random(aMin.x, aMax.x), Random(aMin.y, aMax.y), Random(aMin.z, aMax.z), Random(aMin.w, aMax.w));
}