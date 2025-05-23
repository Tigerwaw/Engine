#include "Enginepch.h"

#include "Timer.h"

Timer::Timer()
{
	myStartTime = std::chrono::high_resolution_clock::now();
	myLastFrameTime = std::chrono::high_resolution_clock::now();
	myCurrentFrameTime = std::chrono::high_resolution_clock::now();
}

void Timer::Update()
{
	PIXScopedEvent(PIX_COLOR_INDEX(2), "Timer Update");
	myLastFrameTime = myCurrentFrameTime;
	myCurrentFrameTime = std::chrono::high_resolution_clock::now();

	myCurrentFPSCountFrame++;
	myTotalFPS += 1 / GetDeltaTime();

	if (myCurrentFPSCountFrame >= myMaxFPSCountFrame)
	{
		myAverageFPS = static_cast<int>(myTotalFPS / myCurrentFPSCountFrame);
		myCurrentFPSCountFrame = 0;
		myTotalFPS = 0;
	}

}

double Timer::GetTimeSinceEpoch() const
{
	return static_cast<double>(std::chrono::system_clock::now().time_since_epoch().count());
}

float Timer::GetDeltaTime() const
{
	const std::chrono::duration<float, std::ratio<1, 1>> deltaTime = myCurrentFrameTime - myLastFrameTime;
	return deltaTime.count() * GetTimeScale();
}

float Timer::GetUnscaledDeltaTime() const
{
	const std::chrono::duration<float, std::ratio<1, 1>> deltaTime = myCurrentFrameTime - myLastFrameTime;
	return deltaTime.count();
}

double Timer::GetTimeSinceProgramStart() const
{
	const std::chrono::duration<double, std::ratio<1, 1>> timeSinceStart = myCurrentFrameTime - myStartTime;
	return timeSinceStart.count();
}

float Timer::GetFrameTimeMS() const
{
	const std::chrono::duration<float, std::ratio<1, 1000>> frameTime = myCurrentFrameTime - myLastFrameTime;
	return frameTime.count() * GetTimeScale();
}

int Timer::GetFPS() const
{
	return myAverageFPS;
}