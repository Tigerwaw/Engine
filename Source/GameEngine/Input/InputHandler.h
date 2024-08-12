#pragma once
#include "GenericInput.h"
#include "MKBInput.h"
#include "ControllerInput.h"
#include "InputAction.h"

#include "Math/Vector.hpp"
namespace CU = CommonUtilities;

class InputHandler
{
public:
	enum class InputMode : int
	{
		MKB,
		Gamepad,
		Count
	};

	InputHandler();
	// Call this from windows message loop
	void UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam);
	// Call this after game update
	void UpdateInput();
	void SetCursorVisibility(bool aIsVisible);

	void SetControllerDeadZone(float aDeadZoneX, float aDeadZoneY);
	void SetLeftStickDeadZone(float aDeadZoneX, float aDeadZoneY);
	void SetRightStickDeadZone(float aDeadZoneX, float aDeadZoneY);
	const CU::Vector2f GetLeftStickDeadZone() const;
	const CU::Vector2f GetRightStickDeadZone() const;

	void RegisterBinaryAction(std::string aActionName, Keys aInput, GenericInput::ActionType aActionType);
	void RegisterBinaryAction(std::string aActionName, ControllerButtons aInput, GenericInput::ActionType aActionType);
	void RegisterAnalogAction(std::string aActionName, Keys aNegativeInput, Keys aPositiveInput);
	void RegisterAnalogAction(std::string aActionName, ControllerButtons aNegativeInput, ControllerButtons aPositiveInput);
	void RegisterAnalogAction(std::string aActionName, MouseMovement aInput);
	void RegisterAnalogAction(std::string aActionName, AnalogInput aInput);
	void RegisterAnalog2DAction(std::string aActionName, MouseMovement2D aInput);
	void RegisterAnalog2DAction(std::string aActionName, AnalogInput2D aInput);

	const bool GetBinaryAction(std::string aActionName) const;
	const float GetAnalogAction(std::string aActionName) const;
	const CU::Vector2f GetAnalogAction2D(std::string aActionName) const;
private:
	const bool ValidateActionName(std::string aActionName, bool aNoLog = false) const;

	std::unordered_map<InputMode, std::shared_ptr<GenericInput>> myInputModes;

	std::unordered_map<std::string, std::vector<std::shared_ptr<InputAction>>> myActions;
};
