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

inline Math::Vector2f Random(Math::Vector2f aMin, Math::Vector2f aMax)
{
	return Math::Vector2f(Random(aMin.x, aMax.x), Random(aMin.y, aMax.y));
}

inline Math::Vector3f Random(Math::Vector3f aMin, Math::Vector3f aMax)
{
	return Math::Vector3f(Random(aMin.x, aMax.x), Random(aMin.y, aMax.y), Random(aMin.z, aMax.z));
}

inline Math::Vector4f Random(Math::Vector4f aMin, Math::Vector4f aMax)
{
	return Math::Vector4f(Random(aMin.x, aMax.x), Random(aMin.y, aMax.y), Random(aMin.z, aMax.z), Random(aMin.w, aMax.w));
}