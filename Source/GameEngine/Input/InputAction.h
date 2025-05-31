#pragma once
#include "Math/Vector.hpp"


#include "GenericInput.h"
#include "MKBInput.h"
#include "ControllerInput.h"

class InputAction
{
public:
    virtual ~InputAction() = default;

    InputAction(std::shared_ptr<GenericInput> aInputHandler, unsigned aKeyCode)
    {
        myKeyCode = aKeyCode;
        myInputHandler = aInputHandler;
    }
    
    unsigned myKeyCode = 0;
    std::shared_ptr<GenericInput> myInputHandler;
};


class BinaryAction : public InputAction
{
public:
    BinaryAction(std::shared_ptr<GenericInput> aInputHandler, unsigned aKeyCode, GenericInput::ActionType aActionType) : InputAction(aInputHandler, aKeyCode)
    {
        myActionType = aActionType;
    }

    const virtual bool GetAction()
    {
        if (!myInputHandler) return false;

        return myInputHandler->GetBinary(myActionType, myKeyCode);
    }
protected:
    GenericInput::ActionType myActionType = GenericInput::ActionType::Clicked;
};

class AnalogAction : public InputAction
{
public:
    AnalogAction(std::shared_ptr<GenericInput> aInputHandler, unsigned aKeyCode) : InputAction(aInputHandler, aKeyCode) {}
    AnalogAction(std::shared_ptr<GenericInput> aInputHandler, unsigned aNegativeKeyCode, unsigned aPositiveKeyCode) : InputAction(aInputHandler, aNegativeKeyCode)
    {
        myKeyCode2 = aPositiveKeyCode;
        myIsBinaryRange = true;
    }

    const virtual float GetAction()
    {
        if (!myInputHandler) return 0;

        if (myIsBinaryRange)
        {
            bool negative = myInputHandler->GetBinary(GenericInput::ActionType::Held, myKeyCode);
            bool positive = myInputHandler->GetBinary(GenericInput::ActionType::Held, myKeyCode2);

            float result = 0;
            if (negative && !positive)
            {
                result = -1.0f;
            }
            else if (!negative && positive)
            {
                result = 1.0f;
            }

            return result;
        }

        return myInputHandler->GetAnalog(myKeyCode);
    }
protected:
    unsigned myKeyCode2 = 0;
    bool myIsBinaryRange = false;
};

class Analog2DAction : public InputAction
{
public:
    Analog2DAction(std::shared_ptr<GenericInput> aInputHandler, unsigned aKeyCode) : InputAction(aInputHandler, aKeyCode) {}

    const virtual Math::Vector2f GetAction()
    {
        if (!myInputHandler) return Math::Vector2f();

        return myInputHandler->GetAnalog2D(myKeyCode);
    }
};