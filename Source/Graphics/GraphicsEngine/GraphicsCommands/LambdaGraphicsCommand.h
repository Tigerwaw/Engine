#pragma once
#include <functional>
#include "GraphicsCommandBase.h"

using GraphicsCommandFunction = std::function<void()>;

struct LambdaGraphicsCommand final : public GraphicsCommandBase
{
	GraphicsCommandFunction myLambda;

	LambdaGraphicsCommand(GraphicsCommandFunction&& aLambda) : myLambda(std::move(aLambda))
	{

	}

	void Execute() override
	{
		myLambda();
	}

	void Destroy() override
	{
		myLambda.~GraphicsCommandFunction();
	}
};

