#pragma once
#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

class GameObject;
class Material;
struct PipelineStateObject;

class ModelViewer : public Application
{
public:
	ModelViewer() {}
	~ModelViewer() {}

	void InitializeApplication() override;
	void UpdateApplication() override;
private:
	void ResetScene();
	void ResetGameObject(std::shared_ptr<GameObject> aGO);
	void ResetMaterial();
	void ResetPSO();
	void SetModel(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath);
	void SetAnimatedModel(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath);
	void SetAnimation(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath);
	void SetMaterial(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath);
	void SetTexture(std::shared_ptr<GameObject> aGO, std::filesystem::path& aAssetPath);
	void SetPSO(std::filesystem::path& aAssetPath);
	void SetShader(std::filesystem::path& aAssetPath);

	void ExportMaterial();
	void ExportPSO();
	void SetupImguiStyle();

	struct Log
	{
		std::string message;
		std::string tooltip;
		CU::Vector3f color = { 1.0f, 1.0f, 1.0f };

		Log(std::string aMessage, CU::Vector3f aColor = CU::Vector3f(1.0f, 1.0f, 1.0f))
		{
			message = aMessage;
			color = aColor;
		}

		Log(std::string aMessage, std::string aTooltip, CU::Vector3f aColor = CU::Vector3f(1.0f, 1.0f, 1.0f))
		{
			message = aMessage;
			tooltip = aTooltip;
			color = aColor;
		}
	};

	std::vector<Log> myLogs;

	std::shared_ptr<Material> myMaterial;
	std::shared_ptr<PipelineStateObject> myPSO;
	bool myIsCustomMaterial = false;

	std::string myMeshName;
	std::string myMeshPath;

	std::string myMaterialName;
	std::string myMaterialPath;
	std::string myAlbedoTexName;
	std::string myAlbedoTexPath;
	std::string myNormalTexName;
	std::string myNormalTexPath;
	std::string myMaterialTexName;
	std::string myMaterialTexPath;

	std::string myPSOName;
	std::string myPSOPath;
	std::string myVertexShaderName;
	std::string myVertexShaderPath;
	std::string myGeometryShaderName;
	std::string myGeometryShaderPath;
	std::string myPixelShaderName;
	std::string myPixelShaderPath;

	ImFont* newFont;
	unsigned currentDebugMode = 0;
};
