#pragma once
#include "ConstantBuffers/MaterialBuffer.h"

struct PipelineStateObject;
class Texture;

class Material
{
public:
	enum class TextureType
	{
		Albedo,
		Normal,
		Material,
		Effects
	};

	Material();
	std::shared_ptr<Material> CreateInstance() const;
	MaterialBuffer& MaterialSettings() { return myMaterialSettings; }

	void SetPSO(std::shared_ptr<PipelineStateObject> aPSO) { myPSO = aPSO; }
	std::shared_ptr<PipelineStateObject> GetPSO() { return myPSO; }

	void SetTexture(TextureType aTextureType, std::shared_ptr<Texture> aTexture);
	Texture& GetTexture(TextureType aTextureType);

private:
	MaterialBuffer myMaterialSettings;
	std::shared_ptr<PipelineStateObject> myPSO;
	std::unordered_map<TextureType, std::shared_ptr<Texture>> myTextures;
};