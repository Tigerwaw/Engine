#pragma once

struct AIEvent
{
	std::string type;
	std::any data = 0;

	AIEvent() = default;
	AIEvent(std::string aType, std::any aData)
	{
		type = aType;
		data = aData;
	}
};