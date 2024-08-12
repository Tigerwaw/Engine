#include "GameEngine/Application.h"
#include "GameEngine/EntryPoint.h"

class ModelViewer : public Application
{
public:
	ModelViewer() {}
    ~ModelViewer() {}

    void InitializeApplication() override;
};

Application* CreateApplication()
{
    return new ModelViewer();
}

void ModelViewer::InitializeApplication()
{
	Engine::GetInstance().GetInputHandler().SetControllerDeadZone(0.1f, 0.06f);
	Engine::GetInstance().GetAudioEngine().Initialize();
	Engine::GetInstance().GetAudioEngine().LoadBank("Master");
	Engine::GetInstance().GetAudioEngine().LoadBank("Master.strings");
	Engine::GetInstance().GetAudioEngine().LoadBank("Test");

	Engine::GetInstance().GetAudioEngine().AddBus(BusType::Music, "Music");
	Engine::GetInstance().GetAudioEngine().AddBus(BusType::Ambience, "Ambience");
	Engine::GetInstance().GetAudioEngine().AddBus(BusType::SFX, "SFX");

	Engine::GetInstance().GetDebugDrawer().InitializeDebugDrawer();
	Engine::GetInstance().GetSceneHandler().LoadScene("Scenes/TestScene.json");

	InputHandler& inputHandler = Engine::GetInstance().GetInputHandler();
	inputHandler.RegisterBinaryAction("W", Keys::W, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("A", Keys::A, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("S", Keys::S, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("D", Keys::D, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("LMB", Keys::MOUSELBUTTON, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("RMB", Keys::MOUSERBUTTON, GenericInput::ActionType::Held);
	inputHandler.RegisterAnalog2DAction("MousePos", MouseMovement2D::MousePos);
	inputHandler.RegisterAnalog2DAction("MouseNDCPos", MouseMovement2D::MousePosNDC);
	inputHandler.RegisterAnalog2DAction("MouseDelta", MouseMovement2D::MousePosDelta);

	inputHandler.RegisterBinaryAction("A_Gamepad", ControllerButtons::A, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("B", ControllerButtons::B, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("X", ControllerButtons::X, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Y", ControllerButtons::Y, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Down", ControllerButtons::DPAD_DOWN, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Left", ControllerButtons::DPAD_LEFT, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Up", ControllerButtons::DPAD_UP, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Right", ControllerButtons::DPAD_RIGHT, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("LS", ControllerButtons::LEFT_STICK_DOWN, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("RS", ControllerButtons::RIGHT_STICK_DOWN, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("LB", ControllerButtons::LEFT_SHOULDER, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("RB", ControllerButtons::RIGHT_SHOULDER, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Start", ControllerButtons::START, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("Back", ControllerButtons::BACK, GenericInput::ActionType::Held);
	inputHandler.RegisterAnalog2DAction("LeftStick", AnalogInput2D::LEFT_STICK);
	inputHandler.RegisterAnalog2DAction("RightStick", AnalogInput2D::RIGHT_STICK);
	inputHandler.RegisterAnalogAction("LeftTrigger", AnalogInput::LEFT_TRIGGER);
	inputHandler.RegisterAnalogAction("RightTrigger", AnalogInput::RIGHT_TRIGGER);

	inputHandler.RegisterBinaryAction("SharedAction", Keys::W, GenericInput::ActionType::Held);
	inputHandler.RegisterBinaryAction("SharedAction", ControllerButtons::A, GenericInput::ActionType::Held);
}
