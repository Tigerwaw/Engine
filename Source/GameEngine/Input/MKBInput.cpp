#include "Enginepch.h"

#include "MKBInput.h"
#include "Engine.h"

MKBInput::MKBInput()
{
	myCurrentBinaryState = std::array<bool, static_cast<int>(Keys::COUNT)>();
	myPreviousBinaryState = std::array<bool, static_cast<int>(Keys::COUNT)>();
	myCurrentAnalogState = std::array<float, static_cast<int>(MouseMovement::COUNT)>();
	myPreviousAnalogState = std::array<float, static_cast<int>(MouseMovement::COUNT)>();
	myDeltaAnalogState = std::array<float, static_cast<int>(MouseMovement::COUNT)>();
}

const void MKBInput::UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
		myCurrentBinaryState[VK_LBUTTON] = true;
		return;
		break;
	case WM_LBUTTONUP:
		myCurrentBinaryState[VK_LBUTTON] = false;
		return;
		break;

	case WM_RBUTTONDOWN:
		myCurrentBinaryState[VK_RBUTTON] = true;
		return;
		break;
	case WM_RBUTTONUP:
		myCurrentBinaryState[VK_RBUTTON] = false;
		return;
		break;

	case WM_MBUTTONDOWN:
		myCurrentBinaryState[VK_MBUTTON] = true;
		return;
		break;
	case WM_MBUTTONUP:
		myCurrentBinaryState[VK_MBUTTON] = false;
		return;
		break;

	case WM_KEYDOWN:
		myCurrentBinaryState[wParam] = true;
		return;
		break;
	case WM_KEYUP:
		myCurrentBinaryState[wParam] = false;
		return;
		break;

	case WM_MOUSEMOVE:
	{
		Math::Vector2f windowSize = Engine::Get().GetWindowSize();
		myCurrentAnalogState[static_cast<int>(MouseMovement::MousePos_X)] = static_cast<float>(GET_X_LPARAM(lParam));
		myCurrentAnalogState[static_cast<int>(MouseMovement::MousePos_Y)] = windowSize.y - static_cast<float>(GET_Y_LPARAM(lParam));

		Math::Vector2f ndcPos;
		myCurrentAnalogState[static_cast<int>(MouseMovement::MousePosNDC_X)] = ((myCurrentAnalogState[static_cast<int>(MouseMovement::MousePos_X)] / windowSize.x) - 0.5f) * 2;
		myCurrentAnalogState[static_cast<int>(MouseMovement::MousePosNDC_Y)] = ((myCurrentAnalogState[static_cast<int>(MouseMovement::MousePos_Y)] / windowSize.y) - 0.5f) * 2;
		return;
		break;
	}
	case WM_MOUSEWHEEL:
		myDeltaAnalogState[static_cast<int>(MouseMovement::ScrollwheelDelta)] = std::clamp(static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)), -1.0f, 1.0f);
		return;
		break;

	default:
		return;
		break;
	}
}

const void MKBInput::UpdateInput()
{
	int index = static_cast<int>(MouseMovement::MousePos_X);
	myDeltaAnalogState[static_cast<int>(MouseMovement::MousePosDelta_X)] = std::clamp(myCurrentAnalogState[index] - myPreviousAnalogState[index], -1.0f, 1.0f);

	index = static_cast<int>(MouseMovement::MousePos_Y);
	myDeltaAnalogState[static_cast<int>(MouseMovement::MousePosDelta_Y)] = std::clamp(myCurrentAnalogState[index] - myPreviousAnalogState[index], -1.0f, 1.0f);

	myDeltaAnalogState[static_cast<int>(MouseMovement::ScrollwheelDelta)] = 0;

	myPreviousBinaryState = myCurrentBinaryState;
	myPreviousAnalogState = myCurrentAnalogState;
}

const bool MKBInput::GetBinary(const GenericInput::ActionType aActionType, const unsigned aKeyCode) const
{
	switch (aActionType)
	{
	case GenericInput::ActionType::Clicked:
		return myCurrentBinaryState[aKeyCode] == true && myPreviousBinaryState[aKeyCode] == false;
		break;
	case GenericInput::ActionType::Held:
		return myCurrentBinaryState[aKeyCode] == true;
		break;
	case GenericInput::ActionType::Released:
		return myCurrentBinaryState[aKeyCode] == false && myPreviousBinaryState[aKeyCode] == true;
		break;
	}

	return false;
}

const float MKBInput::GetAnalog(const unsigned aKeyCode) const
{
	switch (static_cast<MouseMovement>(aKeyCode))
	{
		case MouseMovement::MousePos_X:
			return myCurrentAnalogState[aKeyCode];
			break;
		case MouseMovement::MousePos_Y:
			return myCurrentAnalogState[aKeyCode];
			break;
		case MouseMovement::MousePosNDC_X:
			return myCurrentAnalogState[aKeyCode];
			break;
		case MouseMovement::MousePosNDC_Y:
			return myCurrentAnalogState[aKeyCode];
			break;
		case MouseMovement::MousePosDelta_X:
			return myDeltaAnalogState[aKeyCode];
			break;
		case MouseMovement::MousePosDelta_Y:
			return myDeltaAnalogState[aKeyCode];
			break;
		case MouseMovement::ScrollwheelDelta:
			return myDeltaAnalogState[aKeyCode];
			break;
	}

	return 0;
}

const Math::Vector2f MKBInput::GetAnalog2D(const unsigned aKeyCode) const
{
	switch (static_cast<MouseMovement2D>(aKeyCode))
	{
	case MouseMovement2D::MousePos:
		return { myCurrentAnalogState[static_cast<int>(MouseMovement::MousePos_X)], myCurrentAnalogState[static_cast<int>(MouseMovement::MousePos_Y)] };
		break;
	case MouseMovement2D::MousePosNDC:
		return { myCurrentAnalogState[static_cast<int>(MouseMovement::MousePosNDC_X)], myCurrentAnalogState[static_cast<int>(MouseMovement::MousePosNDC_Y)] };
		break;
	case MouseMovement2D::MousePosDelta:
		return { myDeltaAnalogState[static_cast<int>(MouseMovement::MousePosDelta_X)], myDeltaAnalogState[static_cast<int>(MouseMovement::MousePosDelta_Y)] };
		break;
	}

	return Math::Vector2f();
}