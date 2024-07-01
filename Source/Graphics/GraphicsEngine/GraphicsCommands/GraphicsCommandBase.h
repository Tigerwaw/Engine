#pragma once

struct GraphicsCommandBase
{
	virtual void Execute() = 0;
	virtual void Destroy() = 0;

	GraphicsCommandBase* Next = nullptr;
};

