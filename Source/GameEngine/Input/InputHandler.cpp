#include "Enginepch.h"

#include "InputHandler.h"
#include "Engine.h"

InputHandler::InputHandler()
{
	myInputModes.emplace(InputMode::MKB, std::make_shared<MKBInput>());
	myInputModes.emplace(InputMode::Gamepad, std::make_shared<ControllerInput>());
}

void InputHandler::UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam)
{
	myInputModes[InputMode::MKB]->UpdateEvents(message, wParam, lParam);
}

void InputHandler::UpdateInput()
{
	PIXScopedEvent(PIX_COLOR_INDEX(4), "Update Input");
	for (int i = 0; i < static_cast<int>(InputMode::Count); i++)
	{
		myInputModes[static_cast<InputMode>(i)]->UpdateInput();
	}
}

void InputHandler::SetCursorVisibility(bool aIsVisible)
{
	ShowCursor(aIsVisible);
}

void InputHandler::SetControllerDeadZone(float aDeadZoneX, float aDeadZoneY)
{
	std::dynamic_pointer_cast<ControllerInput>(myInputModes[InputMode::Gamepad])->SetDeadZone(aDeadZoneX, aDeadZoneY);
}

void InputHandler::SetLeftStickDeadZone(float aDeadZoneX, float aDeadZoneY)
{
	std::dynamic_pointer_cast<ControllerInput>(myInputModes[InputMode::Gamepad])->SetLeftStickDeadZone(aDeadZoneX, aDeadZoneY);
}

void InputHandler::SetRightStickDeadZone(float aDeadZoneX, float aDeadZoneY)
{
	std::dynamic_pointer_cast<ControllerInput>(myInputModes[InputMode::Gamepad])->SetRightStickDeadZone(aDeadZoneX, aDeadZoneY);
}

const CU::Vector2f InputHandler::GetLeftStickDeadZone() const
{
	return std::dynamic_pointer_cast<ControllerInput>(myInputModes.at(InputMode::Gamepad))->GetLeftStickDeadZone();
}

const CU::Vector2f InputHandler::GetRightStickDeadZone() const
{
	return std::dynamic_pointer_cast<ControllerInput>(myInputModes.at(InputMode::Gamepad))->GetRightStickDeadZone();
}


void InputHandler::RegisterBinaryAction(const std::string& aActionName, Keys aInput, GenericInput::ActionType aActionType)
{
	if (!ValidateActionName(aActionName, true))
	{
		myActions.emplace(aActionName, std::vector<std::shared_ptr<InputAction>>());
	}

	myActions[aActionName].emplace_back(std::make_shared<BinaryAction>(myInputModes.at(InputMode::MKB), static_cast<unsigned>(aInput), aActionType));
}

void InputHandler::RegisterBinaryAction(const std::string& aActionName, ControllerButtons aInput, GenericInput::ActionType aActionType)
{
	if (!ValidateActionName(aActionName, true))
	{
		myActions.emplace(aActionName, std::vector<std::shared_ptr<InputAction>>());
	}

	myActions[aActionName].emplace_back(std::make_shared<BinaryAction>(myInputModes.at(InputMode::Gamepad), static_cast<unsigned>(aInput), aActionType));
}

void InputHandler::RegisterAnalogAction(const std::string& aActionName, Keys aNegativeInput, Keys aPositiveInput)
{
	if (!ValidateActionName(aActionName, true))
	{
		myActions.emplace(aActionName, std::vector<std::shared_ptr<InputAction>>());
	}

	myActions[aActionName].emplace_back(std::make_shared<AnalogAction>(myInputModes.at(InputMode::MKB), static_cast<unsigned>(aNegativeInput), static_cast<unsigned>(aPositiveInput)));
}

void InputHandler::RegisterAnalogAction(const std::string& aActionName, ControllerButtons aNegativeInput, ControllerButtons aPositiveInput)
{
	if (!ValidateActionName(aActionName, true))
	{
		myActions.emplace(aActionName, std::vector<std::shared_ptr<InputAction>>());
	}

	myActions[aActionName].emplace_back(std::make_shared<AnalogAction>(myInputModes.at(InputMode::Gamepad), static_cast<unsigned>(aNegativeInput), static_cast<unsigned>(aPositiveInput)));
}

void InputHandler::RegisterAnalogAction(const std::string& aActionName, MouseMovement aInput)
{
	if (!ValidateActionName(aActionName, true))
	{
		myActions.emplace(aActionName, std::vector<std::shared_ptr<InputAction>>());
	}

	myActions[aActionName].emplace_back(std::make_shared<AnalogAction>(myInputModes.at(InputMode::MKB), static_cast<unsigned>(aInput)));
}

void InputHandler::RegisterAnalogAction(const std::string& aActionName, ControllerAnalog aInput)
{
	if (!ValidateActionName(aActionName, true))
	{
		myActions.emplace(aActionName, std::vector<std::shared_ptr<InputAction>>());
	}
	
	myActions[aActionName].emplace_back(std::make_shared<AnalogAction>(myInputModes.at(InputMode::Gamepad), static_cast<unsigned>(aInput)));
}

void InputHandler::RegisterAnalog2DAction(const std::string& aActionName, MouseMovement2D aInput)
{
	if (!ValidateActionName(aActionName, true))
	{
		myActions.emplace(aActionName, std::vector<std::shared_ptr<InputAction>>());
	}
	
	myActions[aActionName].emplace_back(std::make_shared<Analog2DAction>(myInputModes.at(InputMode::MKB), static_cast<unsigned>(aInput)));
}

void InputHandler::RegisterAnalog2DAction(const std::string& aActionName, ControllerAnalog2D aInput)
{
	if (!ValidateActionName(aActionName, true))
	{
		myActions.emplace(aActionName, std::vector<std::shared_ptr<InputAction>>());
	}

	myActions[aActionName].emplace_back(std::make_shared<Analog2DAction>(myInputModes.at(InputMode::Gamepad), static_cast<unsigned>(aInput)));
}


const bool InputHandler::GetBinaryAction(const std::string& aActionName) const
{
	if (!ValidateActionName(aActionName)) return false;

	bool result = false;
	for (auto& action : myActions.at(aActionName))
	{
		std::shared_ptr<BinaryAction> castedAction = std::dynamic_pointer_cast<BinaryAction>(action);
		if (!castedAction) continue;
		result = castedAction->GetAction();

		if (result == true)
		{
			break;
		}
	}

	return result;
}

const float InputHandler::GetAnalogAction(const std::string& aActionName) const
{
	if (!ValidateActionName(aActionName)) return false;

	float result = 0;
	for (auto& action : myActions.at(aActionName))
	{
		std::shared_ptr<AnalogAction> castedAction = std::dynamic_pointer_cast<AnalogAction>(action);
		if (!castedAction) continue;
		result = castedAction->GetAction();

		if (abs(result) > 0)
		{
			break;
		}
	}

	return result;
}

const CU::Vector2f InputHandler::GetAnalogAction2D(const std::string& aActionName) const
{
	CU::Vector2f result;
	for (auto& action : myActions.at(aActionName))
	{
		std::shared_ptr<Analog2DAction> castedAction = std::dynamic_pointer_cast<Analog2DAction>(action);
		if (!castedAction) continue;
		result = castedAction->GetAction();

		if (result.LengthSqr() > 0)
		{
			break;
		}
	}

	return result;
}

const bool InputHandler::ValidateActionName(const std::string& aActionName, bool aNoLog) const
{
	auto it = myActions.find(aActionName);
	if (it == myActions.end())
	{
		if (!aNoLog)
		{
			LOG(LogInputHandler, Warning, "Input action {} does not exist!", aActionName);
		}
		return false;
	}

	return true;
}