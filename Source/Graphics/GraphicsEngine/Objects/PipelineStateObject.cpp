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

	instance->Shaders = Shaders;

	return instance;
}

void PipelineStateObject::SetShader(ShaderType aShaderType, std::shared_ptr<Shader> aShader)
{
	Shaders[static_cast<int>(aShaderType)] = aShader;
}

std::shared_ptr<Shader> PipelineStateObject::GetShader(ShaderType aShaderType) const
{
	return Shaders[static_cast<int>(aShaderType)];
}
