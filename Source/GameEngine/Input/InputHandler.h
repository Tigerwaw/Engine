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

	void RegisterBinaryAction(const std::string& aActionName, Keys aInput, GenericInput::ActionType aActionType);
	void RegisterBinaryAction(const std::string& aActionName, ControllerButtons aInput, GenericInput::ActionType aActionType);
	void RegisterAnalogAction(const std::string& aActionName, Keys aNegativeInput, Keys aPositiveInput);
	void RegisterAnalogAction(const std::string& aActionName, ControllerButtons aNegativeInput, ControllerButtons aPositiveInput);
	void RegisterAnalogAction(const std::string& aActionName, MouseMovement aInput);
	void RegisterAnalogAction(const std::string& aActionName, ControllerAnalog aInput);
	void RegisterAnalog2DAction(const std::string& aActionName, MouseMovement2D aInput);
	void RegisterAnalog2DAction(const std::string& aActionName, ControllerAnalog2D aInput);

	const bool GetBinaryAction(const std::string& aActionName) const;
	const float GetAnalogAction(const std::string& aActionName) const;
	const CU::Vector2f GetAnalogAction2D(const std::string& aActionName) const;
private:
	const bool ValidateActionName(const std::string& aActionName, bool aNoLog = false) const;

	std::unordered_map<InputMode, std::shared_ptr<GenericInput>> myInputModes;

	std::unordered_map<std::string, std::vector<std::shared_ptr<InputAction>>> myActions;
};
