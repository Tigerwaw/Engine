#pragma once

enum class Tonemapper
{
	UE,
	ACES,
	Lottes,
	COUNT
};

enum class Luminance
{
	RandomGain,
	Contrast,
	ReductionAndGain,
	Fade,
	COUNT
};

enum class Bloom
{
	Additive,
	ScaledToScene,
	ScaledToLuminance,
	COUNT
};

class PostProcessingSettings
{
public:
	bool Initialize();

	std::vector<std::string> TonemapperNames = {
	"TonemapUE.pso",
	"TonemapACES.pso",
	"TonemapLottes.pso"
	};

	std::vector<std::string> LuminanceNames = {
		"RandomGain",
		"Contrast",
		"ReductionAndGain",
		"Fade"
	};

	std::vector<std::string> BloomNames = {
		"Additive",
		"ScaledToScene",
		"ScaledToLuminance"
	};

	const std::vector<Math::Vector4f>& GetRandomKernel() const { return myRandomKernel; }

	Tonemapper Tonemapper = Tonemapper::UE;
	Bloom BloomFunction = Bloom::ScaledToLuminance;
	Luminance LuminanceFunction = Luminance::Fade;
	bool BloomEnabled = true;
	bool SSAOEnabled = true;
	float BloomStrength = 0.5f;
	float SSAONoisePower = 0.25f;
	float SSAORadius = 0.05f;
	float SSAOBias = 0.025f;
private:
	void CreateRandomKernel(unsigned aKernelSize);
	std::vector<Math::Vector4f> myRandomKernel;
};