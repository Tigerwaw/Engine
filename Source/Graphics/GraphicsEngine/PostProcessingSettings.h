#pragma once
#include "GraphicsSettings.hpp"

class PostProcessingSettings
{
public:
	void CreateRandomKernel(unsigned aKernelSize);
	const std::vector<Math::Vector4f>& GetRandomKernel() const { return myRandomKernel; }

	TonemapperType Tonemapper = TonemapperType::UE;
	BloomType BloomFunction = BloomType::ScaledToLuminance;
	LuminanceType LuminanceFunction = LuminanceType::Fade;
	bool BloomEnabled = true;
	bool SSAOEnabled = true;
	float BloomStrength = 0.5f;
	float SSAONoisePower = 0.25f;
	float SSAORadius = 0.05f;
	float SSAOBias = 0.025f;
private:
	std::vector<Math::Vector4f> myRandomKernel;
};