#include "InputHandler.h"
#include <Windows.h>
#include <Windowsx.h>

namespace CommonUtilities
{ 
	InputHandler::InputHandler()
	{
		myCurrentMousePosition = { 0, 0 };
		myLastMousePosition = { 0, 0 };
		myMouseDelta = { 0, 0 };
		myCurrentScrollwheelValue = 0;
		myLastScrollwheelValue = 0;
		myScrollwheelDelta = 0;
		myCurrentState = std::array<bool, 256>();
		myPreviousState = std::array<bool, 256>();
		myInputState = std::array<bool, 256>();
	}

	bool InputHandler::UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_LBUTTONDOWN:
			myCurrentState[VK_LBUTTON] = true;
			return true;
			break;
		case WM_LBUTTONUP:
			myCurrentState[VK_LBUTTON] = false;
			return true;
			break;

		case WM_RBUTTONDOWN:
			myCurrentState[VK_RBUTTON] = true;
			return true;
			break;
		case WM_RBUTTONUP:
			myCurrentState[VK_RBUTTON] = false;
			return true;
			break;

		case WM_MBUTTONDOWN:
			myCurrentState[VK_MBUTTON] = true;
			return true;
			break;
		case WM_MBUTTONUP:
			myCurrentState[VK_MBUTTON] = false;
			return true;
			break;

		case WM_KEYDOWN:
			myCurrentState[wParam] = true;
			return true;
			break;
		case WM_KEYUP:
			myCurrentState[wParam] = false;
			return true;
			break;

		case WM_MOUSEMOVE:
			myCurrentMousePosition.x = GET_X_LPARAM(lParam);
			myCurrentMousePosition.y = GET_Y_LPARAM(lParam);
			return true;
			break;

		case WM_MOUSEWHEEL:
			myScrollwheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			return true;
			break;

		default:
			return false;
			break;
		}
	}

	bool InputHandler::GetKeyClicked(const Keys aKeyCode) const
	{
		int keyCode = static_cast<int>(aKeyCode);
		return myCurrentState[keyCode] == true && myPreviousState[keyCode] == false;
	}

	bool InputHandler::GetKeyDown(const Keys aKeyCode) const
	{
		int keyCode = static_cast<int>(aKeyCode);
		return myCurrentState[keyCode] == true;
	}

	bool InputHandler::GetKeyReleased(const Keys aKeyCode) const
	{
		int keyCode = static_cast<int>(aKeyCode);
		return myCurrentState[keyCode] == false && myPreviousState[keyCode] == true;
	}

	CU::Vector2<int> InputHandler::GetMousePosition() const
	{
		return myCurrentMousePosition;
	}

	CU::Vector2<int> InputHandler::GetMouseDelta() const
	{
		return myMouseDelta;
	}

	float InputHandler::GetScrollwheelDelta() const
	{
		return myScrollwheelDelta;
	}

	void InputHandler::UpdateInput()
	{
		myMouseDelta.x = myCurrentMousePosition.x - myLastMousePosition.x;
		myMouseDelta.y = myCurrentMousePosition.y - myLastMousePosition.y;
		myScrollwheelDelta = 0;
		myPreviousState = myCurrentState;
		myLastMousePosition = myCurrentMousePosition;
	}
}