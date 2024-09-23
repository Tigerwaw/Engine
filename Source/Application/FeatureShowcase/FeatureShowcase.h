#pragma once
#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

class FeatureShowcase : public Application
{
public:
	FeatureShowcase() {}
	~FeatureShowcase() {}

	void InitializeApplication() override;
	void UpdateApplication() override;
private:
	int currentAnimation = 0;
	std::vector<std::string> animationNames = {
		"Idle",
		"Walk",
		"Run",
		"Wave"
	};

	unsigned currentDebugMode = 0;
	unsigned currentTonemapper = 1;
};