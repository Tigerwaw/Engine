#pragma once

enum class ControllerButtons : int
{
    DPAD_UP = 0,
    DPAD_DOWN = 1,
    DPAD_LEFT = 2,
    DPAD_RIGHT = 3,
    START = 4,
    BACK = 5,
    LEFT_STICK_DOWN = 6,
    RIGHT_STICK_DOWN = 7,
    LEFT_SHOULDER = 8,
    RIGHT_SHOULDER = 9,
    A = 10,
    B = 11,
    X = 12,
    Y = 13,
    COUNT
};

enum class AnalogInput : int
{
    LEFT_TRIGGER = 0,
    RIGHT_TRIGGER = 1,
    LEFT_STICK_X = 2,
    LEFT_STICK_Y = 3,
    RIGHT_STICK_X = 4,
    RIGHT_STICK_Y = 5,
    COUNT
};

enum class AnalogInput2D : int
{
    LEFT_STICK = 0,
    RIGHT_STICK = 1,
    COUNT
};