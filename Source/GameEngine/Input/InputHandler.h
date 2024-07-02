#pragma once
#include <array>
#include "Math/Vector.hpp"
#include "EnumKeys.h"

typedef unsigned UINT;
typedef unsigned __int64 WPARAM;
typedef __int64 LPARAM;

namespace CommonUtilities
{
	namespace CU = CommonUtilities;

	class InputHandler
	{
	public:
		InputHandler();
		// Call this from windows message loop
		bool UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam);
		// Call this after game update
		void UpdateInput();

		// Key was active this frame but not the frame before
		bool GetKeyClicked(const Keys aKeyCode) const;
		// Key was active this frame, last frame not taken into account
		bool GetKeyDown(const Keys aKeyCode) const;
		// Key was active last frame, but not this frame
		bool GetKeyReleased(const Keys aKeyCode) const;
		CU::Vector2<int> GetMousePosition() const;
		CU::Vector2<int> GetMouseDelta() const;
		float GetScrollwheelDelta() const;
	private:
		CU::Vector2<int> myCurrentMousePosition;
		CU::Vector2<int> myLastMousePosition;
		CU::Vector2<int> myMouseDelta;
		float myCurrentScrollwheelValue;
		float myLastScrollwheelValue;
		float myScrollwheelDelta;
		std::array<bool, 256> myCurrentState;
		std::array<bool, 256> myPreviousState;
		std::array<bool, 256> myInputState;
	};
}
