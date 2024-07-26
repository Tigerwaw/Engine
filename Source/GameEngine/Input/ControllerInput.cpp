#include "ControllerInput.h"
#include <Windows.h>
#include <Xinput.h>

#pragma comment(lib,"XInput.lib")
#pragma comment(lib,"Xinput9_1_0.lib")

ControllerInput::ControllerInput()
{
    myCurrentBinaryState = std::array<bool, static_cast<int>(ControllerButtons::COUNT)>();
    myPreviousBinaryState = std::array<bool, static_cast<int>(ControllerButtons::COUNT)>();
    myCurrentAnalogState = std::array<float, static_cast<int>(AnalogInput::COUNT)>();
    myPreviousAnalogState = std::array<float, static_cast<int>(AnalogInput::COUNT)>();
    myDeltaAnalogState = std::array<float, static_cast<int>(AnalogInput::COUNT)>();
}

const void ControllerInput::UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam)
{
    message;
    wParam;
    lParam;
}

const void ControllerInput::UpdateInput()
{
    DWORD dwResult;
    for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
    {
        XINPUT_STATE state = {};
        dwResult = XInputGetState(i, &state);

        if (dwResult == 0)
        {
            int input = static_cast<int>(AnalogInput::LEFT_TRIGGER);
            myPreviousAnalogState[input] = myCurrentAnalogState[input];
            myCurrentAnalogState[input] = static_cast<float>(state.Gamepad.bLeftTrigger) / 255;
            myDeltaAnalogState[input] = myCurrentAnalogState[input] - myPreviousAnalogState[input];

            input = static_cast<int>(AnalogInput::RIGHT_TRIGGER);
            myPreviousAnalogState[input] = myCurrentAnalogState[input];
            myCurrentAnalogState[input] = static_cast<float>(state.Gamepad.bRightTrigger) / 255;
            myDeltaAnalogState[input] = myCurrentAnalogState[input] - myPreviousAnalogState[input];

            input = static_cast<int>(AnalogInput::LEFT_STICK_X);
            myPreviousAnalogState[input] = myCurrentAnalogState[input];
            float value = std::fmaxf(-1, static_cast<float>(state.Gamepad.sThumbLX) / 32767);
            myCurrentAnalogState[input] = (abs(value) < myLeftStickDeadZone.x ? 0 : (abs(value) - myLeftStickDeadZone.x) * (value / abs(value)));
            if (myLeftStickDeadZone.x > 0) myCurrentAnalogState[input] /= 1 - myLeftStickDeadZone.x;
            myDeltaAnalogState[input] = myCurrentAnalogState[input] - myPreviousAnalogState[input];

            input = static_cast<int>(AnalogInput::LEFT_STICK_Y);
            myPreviousAnalogState[input] = myCurrentAnalogState[input];
            value = std::fmaxf(-1, static_cast<float>(state.Gamepad.sThumbLY) / 32767);
            myCurrentAnalogState[input] = (abs(value) < myLeftStickDeadZone.y ? 0 : (abs(value) - myLeftStickDeadZone.y) * (value / abs(value)));
            if (myLeftStickDeadZone.y > 0) myCurrentAnalogState[input] /= 1 - myLeftStickDeadZone.y;
            myDeltaAnalogState[input] = myCurrentAnalogState[input] - myPreviousAnalogState[input];

            input = static_cast<int>(AnalogInput::RIGHT_STICK_X);
            myPreviousAnalogState[input] = myCurrentAnalogState[input];
            value = std::fmaxf(-1, static_cast<float>(state.Gamepad.sThumbRX) / 32767);
            myCurrentAnalogState[input] = (abs(value) < myRightStickDeadZone.x ? 0 : (abs(value) - myRightStickDeadZone.x) * (value / abs(value)));
            if (myRightStickDeadZone.x > 0) myCurrentAnalogState[input] /= 1 - myRightStickDeadZone.x;
            myDeltaAnalogState[input] = myCurrentAnalogState[input] - myPreviousAnalogState[input];

            input = static_cast<int>(AnalogInput::RIGHT_STICK_Y);
            myPreviousAnalogState[input] = myCurrentAnalogState[input];
            value = std::fmaxf(-1, static_cast<float>(state.Gamepad.sThumbRY) / 32767);
            myCurrentAnalogState[input] = (abs(value) < myRightStickDeadZone.y ? 0 : (abs(value) - myRightStickDeadZone.y) * (value / abs(value)));
            if (myRightStickDeadZone.y > 0) myCurrentAnalogState[input] /= 1 - myRightStickDeadZone.y;
            myDeltaAnalogState[input] = myCurrentAnalogState[input] - myPreviousAnalogState[input];

            int button = static_cast<int>(ControllerButtons::DPAD_UP);
            myPreviousBinaryState[button] = myCurrentBinaryState[button];
            myCurrentBinaryState[button] = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0);

            button = static_cast<int>(ControllerButtons::DPAD_DOWN);
            myPreviousBinaryState[button] = myCurrentBinaryState[button];
            myCurrentBinaryState[button] = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0);

            button = static_cast<int>(ControllerButtons::DPAD_LEFT);
            myPreviousBinaryState[button] = myCurrentBinaryState[button];
            myCurrentBinaryState[button] = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0);

            button = static_cast<int>(ControllerButtons::DPAD_RIGHT);
            myPreviousBinaryState[button] = myCurrentBinaryState[button];
            myCurrentBinaryState[button] = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0);

            button = static_cast<int>(ControllerButtons::START);
            myPreviousBinaryState[button] = myCurrentBinaryState[button];
            myCurrentBinaryState[button] = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0);

            button = static_cast<int>(ControllerButtons::BACK);
            myPreviousBinaryState[button] = myCurrentBinaryState[button];
            myCurrentBinaryState[button] = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0);

            button = static_cast<int>(ControllerButtons::LEFT_STICK_DOWN);
            myPreviousBinaryState[button] = myCurrentBinaryState[button];
            myCurrentBinaryState[button] = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0);

            button = static_cast<int>(ControllerButtons::RIGHT_STICK_DOWN);
            myPreviousBinaryState[button] = myCurrentBinaryState[button];
            myCurrentBinaryState[button] = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0);

            button = static_cast<int>(ControllerButtons::LEFT_SHOULDER);
            myPreviousBinaryState[button] = myCurrentBinaryState[button];
            myCurrentBinaryState[button] = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0);

            button = static_cast<int>(ControllerButtons::RIGHT_SHOULDER);
            myPreviousBinaryState[button] = myCurrentBinaryState[button];
            myCurrentBinaryState[button] = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0);

            button = static_cast<int>(ControllerButtons::A);
            myPreviousBinaryState[button] = myCurrentBinaryState[button];
            myCurrentBinaryState[button] = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0);

            button = static_cast<int>(ControllerButtons::B);
            myPreviousBinaryState[button] = myCurrentBinaryState[button];
            myCurrentBinaryState[button] = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0);

            button = static_cast<int>(ControllerButtons::X);
            myPreviousBinaryState[button] = myCurrentBinaryState[button];
            myCurrentBinaryState[button] = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0);

            button = static_cast<int>(ControllerButtons::Y);
            myPreviousBinaryState[button] = myCurrentBinaryState[button];
            myCurrentBinaryState[button] = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0);
        }
        else
        {
            // Controller is not connected
        }
    }
}

const bool ControllerInput::GetBinary(const GenericInput::ActionType aActionType, const unsigned aKeyCode) const
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

const float ControllerInput::GetAnalog(const unsigned aKeyCode) const
{
    switch (static_cast<AnalogInput>(aKeyCode))
    {
    case AnalogInput::LEFT_TRIGGER:
        return myCurrentAnalogState[aKeyCode];
        break;
    case AnalogInput::RIGHT_TRIGGER:
        return myCurrentAnalogState[aKeyCode];
        break;
    case AnalogInput::LEFT_STICK_X:
        return myCurrentAnalogState[aKeyCode];
        break;
    case AnalogInput::LEFT_STICK_Y:
        return myCurrentAnalogState[aKeyCode];
        break;
    case AnalogInput::RIGHT_STICK_X:
        return myDeltaAnalogState[aKeyCode];
        break;
    case AnalogInput::RIGHT_STICK_Y:
        return myDeltaAnalogState[aKeyCode];
        break;
    }

    return 0;
}

const CU::Vector2f ControllerInput::GetAnalog2D(const unsigned aKeyCode) const
{
    switch (static_cast<AnalogInput2D>(aKeyCode))
    {
    case AnalogInput2D::LEFT_STICK:
        return { myCurrentAnalogState[static_cast<int>(AnalogInput::LEFT_STICK_X)], myCurrentAnalogState[static_cast<int>(AnalogInput::LEFT_STICK_Y)] };
        break;
    case AnalogInput2D::RIGHT_STICK:
        return { myCurrentAnalogState[static_cast<int>(AnalogInput::RIGHT_STICK_X)], myCurrentAnalogState[static_cast<int>(AnalogInput::RIGHT_STICK_Y)] };
        break;
    }

    return CU::Vector2f();
}

void ControllerInput::SetDeadZone(float aDeadZoneX, float aDeadZoneY)
{
    SetLeftStickDeadZone(aDeadZoneX, aDeadZoneY);
    SetRightStickDeadZone(aDeadZoneX, aDeadZoneY);
}

void ControllerInput::SetLeftStickDeadZone(float aDeadZoneX, float aDeadZoneY)
{
    myLeftStickDeadZone = { aDeadZoneX, aDeadZoneY };
}

void ControllerInput::SetRightStickDeadZone(float aDeadZoneX, float aDeadZoneY)
{
    myRightStickDeadZone = { aDeadZoneX, aDeadZoneY };
}
