#pragma once

struct ID3D11ShaderReflectionType;


/**
 * D3D11 Shader Reflection Wrapper
 * By Daniel Borgshammar @ TGA
 */
struct ShaderInfo
{
	/**
	 * Retrieves Shader Reflection data from the provided shader bytecode.
	 * @param aShaderData A pointer to raw shader bytecode.
	 * @param aShaderDataSize The size of the data pointed to by aShaderData.
	 * @return A ShaderInfo object, if successful.
	 */
	static ShaderInfo Reflect(const uint8_t* aShaderData, size_t aShaderDataSize);

	enum class ShaderType : unsigned
	{
		PixelShader = 0,
		VertexShader = 1,
		GeometryShader = 2,
		HullShader = 3,
		DomainShader = 4,
		ComputeShader = 5,
		Unknown
	};

	enum class ComponentType : unsigned
	{
		Unknown = 0,
		UInt = 1,
		Int = 2,
		Float = 3,
		UInt16 = 4,
		Int16 = 5,
		Half = 6,
		UInt64 = 7,
		Int64 = 8,
		Double = 9,
	};

	struct VariableInfo
	{
		// Name of the variable
		std::string Name;
		// The HLSL type name
		std::string Type;
		// HLSL type size
		size_t Size;
		// Offset where we are in the cbuffer
		size_t Offset;
		// Default value, if present
		uint8_t Default[64]{};
	};

	struct ConstantBufferInfo
	{
		std::string Name;
		size_t Size;
		unsigned Slot;
		std::vector<VariableInfo> Members;
		std::unordered_map<std::string, size_t> VariableNameToIndex;
	};

	struct TextureInfo
	{
		// The texture slot name
		std::string Name;
		// The slot register
		unsigned Slot;
		// Texture dimensions (1D, 2D, 3D)
		unsigned Dimensions;
	};

	struct SamplerInfo
	{
		std::string Name;
		unsigned Slot;
	};

	struct ParameterInfo
	{
		unsigned Register;
		std::string SemanticName;
		unsigned SemanticIndex;
		ComponentType Type;

		bool operator==(const ParameterInfo& aOtherParam) const
		{
			return (Register == aOtherParam.Register
				&& SemanticName == aOtherParam.SemanticName
				&& SemanticIndex == aOtherParam.SemanticIndex
				&& Type == aOtherParam.Type);
		}
	};

	FORCEINLINE ShaderType GetType() const { return myType; }
	FORCEINLINE unsigned GetInstructionCount() const { return myInstructionCount; }
	FORCEINLINE const std::vector<ConstantBufferInfo>& GetConstantBufferInfos() const { return myConstantBuffers; }
	FORCEINLINE const std::vector<TextureInfo>& GetTextureSlots() const { return myTextureSlots; }
	FORCEINLINE const std::vector<SamplerInfo>& GetSamplerSlots() const { return mySamplerSlots; }
	FORCEINLINE const std::vector<ParameterInfo>& GetInputParameters() const { return myInputParameters; }
	FORCEINLINE const std::vector<ParameterInfo>& GetOutputParameters() const { return myOutputParameters; }

	bool HasConstantBufferInfo(const std::string& aConstantBufferName) const;
	const ConstantBufferInfo& GetConstantBufferInfo(const std::string& aConstantBufferName) const;

private:

	ShaderType myType = ShaderType::Unknown;
	unsigned myInstructionCount = 0;
	std::vector<ConstantBufferInfo> myConstantBuffers;
	std::unordered_map<std::string, size_t> myConstantBufferNameToIndex;
	std::vector<TextureInfo> myTextureSlots;
	std::vector<SamplerInfo> mySamplerSlots;
	std::vector<ParameterInfo> myInputParameters;
	std::vector<ParameterInfo> myOutputParameters;
};
