#pragma once
#include "Graphics/GraphicsEngine/GraphicsCommands/GraphicsCommandBase.h"
#include <vector>
#include <memory>

class DynamicVertexBuffer;
struct DebugLineVertex;

class RenderDebugLines : public GraphicsCommandBase
{
public:
	RenderDebugLines(std::vector<DebugLineVertex> aLineVertices, std::shared_ptr<DynamicVertexBuffer> aLineBuffer);
	void Execute() override;
	void Destroy() override;
private:
	std::vector<DebugLineVertex> myLineVertices;
	std::shared_ptr<DynamicVertexBuffer> myLineBuffer;
};
