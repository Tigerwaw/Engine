#pragma once
#include <filesystem>
#include "ConstantBuffers/MaterialBuffer.h"

struct PipelineStateObject;
class Texture;

class Material
{
public:
	Material();
	MaterialBuffer& MaterialSettings() { return myMaterialSettings; }
	std::shared_ptr<PipelineStateObject> GetPSO() { return myPSO; }
	Texture& GetAlbedoTexture() { return *myAlbedoTexture; }
	Texture& GetNormalTexture() { return *myNormalTexture; }
	Texture& GetMaterialTexture() { return *myMaterialTexture; }

	void SetPSO(std::shared_ptr<PipelineStateObject> aPSO) { myPSO = aPSO; }
	void SetAlbedoTexture(std::shared_ptr<Texture> aTexture) { myAlbedoTexture = aTexture; }
	void SetNormalTexture(std::shared_ptr<Texture> aTexture) { myNormalTexture = aTexture; }
	void SetMaterialTexture(std::shared_ptr<Texture> aTexture) { myMaterialTexture = aTexture; }

private:
	MaterialBuffer myMaterialSettings;
	std::shared_ptr<PipelineStateObject> myPSO;
	std::shared_ptr<Texture> myAlbedoTexture;
	std::shared_ptr<Texture> myNormalTexture;
	std::shared_ptr<Texture> myMaterialTexture;
};