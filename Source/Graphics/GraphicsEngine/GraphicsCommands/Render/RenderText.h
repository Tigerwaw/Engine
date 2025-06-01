#pragma once
#include <memory>
#include "GraphicsCommands/GraphicsCommandBase.h"

class Text;

class RenderText : public GraphicsCommandBase
{
public:
	struct TextData
	{
		std::shared_ptr<Text> text;
	};

	RenderText(const TextData& aTextData);
	void Execute() override;
	void Destroy() override;
private:
	TextData myData;
};