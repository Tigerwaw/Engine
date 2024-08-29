#pragma once
#include <memory>
#include "Graphics/GraphicsEngine/GraphicsCommands/GraphicsCommandBase.h"

class Text;

class RenderText : public GraphicsCommandBase
{
public:
	RenderText(std::shared_ptr<Text> aTextObject);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<Text> text;
};