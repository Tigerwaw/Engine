#pragma once
#include "Graphics/GraphicsEngine/GraphicsCommands/GraphicsCommandBase.h"
#include <vector>
#include "GameEngine/Math/Vector.hpp"

namespace CU = CommonUtilities;

struct DebugLine;

class UpdateDebugBuffer : public GraphicsCommandBase
{
public:
	UpdateDebugBuffer(std::vector<DebugLine> aLineList);
	void Execute() override;
	void Destroy() override;
private:
	std::vector<DebugLine> myLineList;
};

