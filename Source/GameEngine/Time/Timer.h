#pragma once
#include <chrono>

namespace CommonUtilities
{
class Timer
{
public:
	Timer();
	Timer(const Timer& aTimer) = delete;
	Timer& operator=(const Timer& aTimer) = delete;
	void Update();
	void SetTimeScale(float aTimeScale) { myTimeScale = aTimeScale; }
	const float GetTimeScale() const { return myTimeScale; }
	float GetDeltaTime() const;
	float GetUnscaledDeltaTime() const;
	double GetTotalTime() const;
	float GetFrameTimeMS() const;
	int GetFPS() const;
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> myStartTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> myLastFrameTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> myCurrentFrameTime;

	int myCurrentFPSCountFrame = 0;
	int myMaxFPSCountFrame = 100;
	float myTotalFPS = 0;
	int myAverageFPS = 0;
	float myTimeScale = 1.0f;
};
}