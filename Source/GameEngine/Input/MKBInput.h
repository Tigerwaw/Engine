#pragma once
#include "GenericInput.h"
#include "Math/Vector.hpp"

#include "EnumKeys.h"

typedef unsigned UINT;
typedef unsigned __int64 WPARAM;
typedef __int64 LPARAM;

class MKBInput : public GenericInput
{
public:
	MKBInput();
	const void UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam) override;
	const void UpdateInput() override;

	const bool GetBinary(const GenericInput::ActionType aActionType, const unsigned aKeyCode) const override;
	const float GetAnalog(const unsigned aKeyCode) const override;
	const Math::Vector2f GetAnalog2D(const unsigned aKeyCode) const override;
private:
	std::array<bool, static_cast<int>(Keys::COUNT)> myCurrentBinaryState;
	std::array<bool, static_cast<int>(Keys::COUNT)> myPreviousBinaryState;
	std::array<float, static_cast<int>(MouseMovement::COUNT)> myCurrentAnalogState;
	std::array<float, static_cast<int>(MouseMovement::COUNT)> myPreviousAnalogState;
	std::array<float, static_cast<int>(MouseMovement::COUNT)> myDeltaAnalogState;
};
