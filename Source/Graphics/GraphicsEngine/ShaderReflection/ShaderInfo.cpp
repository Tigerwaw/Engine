#include "GraphicsEngine.pch.h"
#include "ShaderInfo.h"

namespace ShaderInfoInternal
{
	void ReflectVariable(std::string_view aDomain, ID3D11ShaderReflectionType* aVarType, const D3D11_SHADER_VARIABLE_DESC* aVarDesc, const D3D11_SHADER_TYPE_DESC& aVarTypeDesc, ShaderInfo::ConstantBufferInfo& inoutBufferInfo, size_t& inoutOffset)
	{
		if(aVarTypeDesc.Members > 0)
		{
			// This is a struct or class or similar. It has child members.
			for(unsigned m = 0; m < aVarTypeDesc.Members; ++m)
			{
				ID3D11ShaderReflectionType* memberType = aVarType->GetMemberTypeByIndex(m);

				D3D11_SHADER_TYPE_DESC memberTypeDesc = {};
				memberType->GetDesc(&memberTypeDesc);

				const std::string memberName = aVarType->GetMemberTypeName(m);
				const std::string memberDomain = aDomain.empty() ? memberName : std::string(aDomain) + "." + memberName;
				// Passing nullptr for Desc here since struct members are not allowed to have default values in HLSL.
				ReflectVariable(memberDomain, memberType, nullptr, memberTypeDesc, inoutBufferInfo, inoutOffset);
			}
		}
		else
		{
			ShaderInfo::VariableInfo varInfo;
			varInfo.Name = aDomain;
			varInfo.Type = aVarTypeDesc.Name;
			varInfo.Size = static_cast<size_t>(aVarTypeDesc.Rows * aVarTypeDesc.Columns) * sizeof(float);
			if(aVarDesc && aVarDesc->DefaultValue != nullptr)
			{
				memcpy_s(varInfo.Default, 64, aVarDesc->DefaultValue, aVarDesc->Size);
			}
			varInfo.Offset = inoutOffset;
			inoutOffset += varInfo.Size;

			inoutBufferInfo.VariableNameToIndex.emplace(varInfo.Name, inoutBufferInfo.Members.size());
			inoutBufferInfo.Members.emplace_back(varInfo);
		}
	}
}

ShaderInfo ShaderInfo::Reflect(const uint8_t* aShaderData, size_t aShaderDataSize)
{
	ShaderInfo result;
	
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> shaderReflection;
	const HRESULT reflectResult = D3DReflect(aShaderData, aShaderDataSize, IID_ID3D11ShaderReflection, reinterpret_cast<void**>(shaderReflection.GetAddressOf()));
	if(FAILED(reflectResult))
	{		
		return {};
	}

	D3D11_SHADER_DESC shaderDesc = {};
	shaderReflection->GetDesc(&shaderDesc);
	for(unsigned i = 0; i < shaderDesc.BoundResources; i++)
	{
		D3D11_SHADER_INPUT_BIND_DESC desc = {};
		shaderReflection->GetResourceBindingDesc(i, &desc);
		switch(desc.Type)
		{
		case D3D_SIT_SAMPLER:
			{
				SamplerInfo sampler;
				sampler.Name = desc.Name;
				sampler.Slot = desc.BindPoint;
				result.mySamplerSlots.emplace_back(sampler);
				break;
			}
		case D3D_SIT_TEXTURE:
			{
				TextureInfo texture;
				texture.Name = desc.Name;
				texture.Slot = desc.BindPoint;
				switch(desc.Dimension)
				{
				case D3D_SRV_DIMENSION_TEXTURE1D:
					{
						texture.Dimensions = 1;
						break;
					}
				case D3D_SRV_DIMENSION_TEXTURE2D:
					{
						texture.Dimensions = 2;
						break;
					}
				case D3D_SRV_DIMENSION_TEXTURE3D:
					{
						texture.Dimensions = 3;
						break;
					}
				default:
					{
						texture.Dimensions = 0;
						break;
					}
				}

				result.myTextureSlots.emplace_back(texture);

				break;
			}
			default:
				break;
		}
		std::string test = desc.Name;
	}

	std::ranges::sort(result.myTextureSlots,
	                  [](const TextureInfo& A, const TextureInfo& B){
		                  return A.Slot < B.Slot;
	                  });

	std::ranges::sort(result.mySamplerSlots,
	                  [](const SamplerInfo& A, const SamplerInfo& B){
		                  return A.Slot < B.Slot;
	                  });


	for (size_t i = 0; i < shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC inputParams = {};
		shaderReflection->GetInputParameterDesc(static_cast<UINT>(i), &inputParams);

		if (inputParams.SystemValueType != D3D_NAME_UNDEFINED) continue;

		ParameterInfo paramInfo;
		paramInfo.Register = static_cast<unsigned>(inputParams.Register);
		paramInfo.SemanticName = inputParams.SemanticName;
		paramInfo.SemanticIndex = inputParams.SemanticIndex;
		paramInfo.Type = static_cast<ComponentType>(inputParams.ComponentType);

		result.myInputParameters.emplace_back(paramInfo);
	}

	for (size_t i = 0; i < shaderDesc.OutputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC outputParams = {};
		shaderReflection->GetOutputParameterDesc(static_cast<UINT>(i), &outputParams);

		if (outputParams.SystemValueType != D3D_NAME_UNDEFINED) continue;

		ParameterInfo paramInfo;
		paramInfo.Register = static_cast<unsigned>(outputParams.Register);
		paramInfo.SemanticName = outputParams.SemanticName;
		paramInfo.SemanticIndex = outputParams.SemanticIndex;
		paramInfo.Type = static_cast<ComponentType>(outputParams.ComponentType);

		result.myOutputParameters.emplace_back(paramInfo);
	}


	result.myInstructionCount = shaderDesc.InstructionCount;
	result.myConstantBuffers.reserve(shaderDesc.ConstantBuffers);
	result.myType = static_cast<ShaderType>(D3D11_SHVER_GET_TYPE(shaderDesc.Version));

	for(unsigned c = 0; c < shaderDesc.ConstantBuffers; ++c)
	{
		ShaderInfo::ConstantBufferInfo bufferInfo;
		ID3D11ShaderReflectionConstantBuffer* cbufferReflection = shaderReflection->GetConstantBufferByIndex(c);

		D3D11_SHADER_BUFFER_DESC cbufferDesc = {};
		D3D11_SHADER_INPUT_BIND_DESC cbufferBindDesc = {};

		cbufferReflection->GetDesc(&cbufferDesc);
		shaderReflection->GetResourceBindingDescByName(cbufferDesc.Name, &cbufferBindDesc);

		bufferInfo.Size = cbufferDesc.Size;
		bufferInfo.Name = cbufferDesc.Name;
		bufferInfo.Slot = cbufferBindDesc.BindPoint;

		bufferInfo.Members.reserve(cbufferDesc.Variables);
		bufferInfo.VariableNameToIndex.reserve(cbufferDesc.Variables);

		size_t offset = 0;
		for(unsigned v = 0; v < cbufferDesc.Variables; ++v)
		{
			ID3D11ShaderReflectionVariable* var = cbufferReflection->GetVariableByIndex(v);

			D3D11_SHADER_VARIABLE_DESC varDesc = {};
			var->GetDesc(&varDesc);

			ID3D11ShaderReflectionType* varType = var->GetType();

			D3D11_SHADER_TYPE_DESC varTypeDesc = {};
			varType->GetDesc(&varTypeDesc);
			ShaderInfoInternal::ReflectVariable(varDesc.Name, varType, &varDesc, varTypeDesc, bufferInfo, offset);
		}

		result.myConstantBufferNameToIndex.emplace(bufferInfo.Name, result.myConstantBuffers.size());
		result.myConstantBuffers.emplace_back(std::move(bufferInfo));
	}

	return result;
}

bool ShaderInfo::HasConstantBufferInfo(const std::string& aConstantBufferName) const
{
	return myConstantBufferNameToIndex.contains(aConstantBufferName);
}

const ShaderInfo::ConstantBufferInfo& ShaderInfo::GetConstantBufferInfo(const std::string& aConstantBufferName) const
{
	if(!myConstantBufferNameToIndex.contains(aConstantBufferName))
	{
		throw std::invalid_argument("That Constant Buffer does not exist in this Shader Info!");
	}

	return myConstantBuffers[myConstantBufferNameToIndex.at(aConstantBufferName)];
}
