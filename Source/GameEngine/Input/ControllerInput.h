#pragma once
#include "GenericInput.h"
#include <array>
#include "Math/Vector.hpp"
namespace CU = CommonUtilities;
#include "EnumControllerButtons.h"

typedef unsigned UINT;
typedef unsigned __int64 WPARAM;
typedef __int64 LPARAM;

class ControllerInput : public GenericInput
{
public:
    ControllerInput();
    const void UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam) override;
    const void UpdateInput() override;

    const bool GetBinary(const GenericInput::ActionType aActionType, const unsigned aKeyCode) const override;
    const float GetAnalog(const unsigned aKeyCode) const override;
    const CU::Vector2f GetAnalog2D(const unsigned aKeyCode) const override;

    void SetDeadZone(float aDeadZoneX, float aDeadZoneY);
    void SetLeftStickDeadZone(float aDeadZoneX, float aDeadZoneY);
    void SetRightStickDeadZone(float aDeadZoneX, float aDeadZoneY);
    const CU::Vector2f GetLeftStickDeadZone() const { return myLeftStickDeadZone; }
    const CU::Vector2f GetRightStickDeadZone() const { return myRightStickDeadZone; }
private:
    std::array<bool, static_cast<int>(ControllerButtons::COUNT)> myCurrentBinaryState;
    std::array<bool, static_cast<int>(ControllerButtons::COUNT)> myPreviousBinaryState;
    std::array<float, static_cast<int>(AnalogInput::COUNT)> myCurrentAnalogState;
    std::array<float, static_cast<int>(AnalogInput::COUNT)> myPreviousAnalogState;
    std::array<float, static_cast<int>(AnalogInput::COUNT)> myDeltaAnalogState;

    CU::Vector2f myLeftStickDeadZone;
    CU::Vector2f myRightStickDeadZone;
};