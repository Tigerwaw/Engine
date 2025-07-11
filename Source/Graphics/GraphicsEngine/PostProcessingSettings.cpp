#include "GraphicsEngine.pch.h"
#include "PostProcessingSettings.h"

void PostProcessingSettings::CreateRandomKernel(unsigned aKernelSize)
{
	myRandomKernel.resize(aKernelSize);

	std::uniform_real_distribution<float> randomValues(0, 1);
	std::default_random_engine rng;

	for (unsigned i = 0; i < aKernelSize; ++i)
	{
		Math::Vector4f v = {
		randomValues(rng) * 2.0f - 1.0f,
		randomValues(rng) * 2.0f - 1.0f,
		randomValues(rng),
		0
		};
		v = v.GetNormalized();
		float s = static_cast<float>(i) / static_cast<float>(aKernelSize);
		s = std::lerp(0.1f, 1.0f, s * s);
		v *= s;
		myRandomKernel[i] = v;
	}
}