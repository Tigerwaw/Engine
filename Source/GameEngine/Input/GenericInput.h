#pragma once
#include "Math/Vector.hpp"


typedef unsigned UINT;
typedef unsigned __int64 WPARAM;
typedef __int64 LPARAM;

class GenericInput
{
public:
	enum class ActionType
	{
		Clicked,	// Action was active this frame but not the frame before
		Held,		// Action was active this frame, last frame not taken into account
		Released	// Action was active last frame, but not this frame
	};

	const virtual void UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam) = 0;
	const virtual void UpdateInput() = 0;
	const virtual bool GetBinary(const ActionType aActionType, const unsigned aKeyCode) const = 0;
	const virtual float GetAnalog(const unsigned aKeyCode) const = 0;
	const virtual Math::Vector2f GetAnalog2D(const unsigned aKeyCode) const = 0;
};

