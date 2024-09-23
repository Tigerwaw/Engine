#include "GraphicsEngine.pch.h"
#include "PipelineStateObject.h"
#include "Texture.h"
#include "Shader.h"

PipelineStateObject::PipelineStateObject() = default;
PipelineStateObject::~PipelineStateObject() = default;

std::shared_ptr<PipelineStateObject> PipelineStateObject::CreateInstance() const
{
	std::shared_ptr<PipelineStateObject> instance = std::make_shared<PipelineStateObject>();
	instance->BlendState = BlendState;
	instance->RasterizerState = RasterizerState;
	instance->DepthStencilState = DepthStencilState;
	instance->InputLayout = InputLayout;
	instance->SamplerStates = SamplerStates;
	instance->VertexStride = VertexStride;
	instance->VertexShader = VertexShader;
	instance->GeometryShader = GeometryShader;
	instance->PixelShader = PixelShader;

	return instance;
}
