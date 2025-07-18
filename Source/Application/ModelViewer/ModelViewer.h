#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"
#include "Objects/Shader.h"

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
		Math::Vector3f color = { 1.0f, 1.0f, 1.0f };

		Log(const std::string& aMessage, Math::Vector3f aColor = Math::Vector3f(1.0f, 1.0f, 1.0f))
		{
			message = aMessage;
			color = aColor;
		}

		Log(const std::string& aMessage, const std::string& aTooltip, Math::Vector3f aColor = Math::Vector3f(1.0f, 1.0f, 1.0f))
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
	bool myIsCustomPSO = false;

	std::string myMeshName;
	std::string myMeshPath;

	std::string myMaterialName;
	std::string myMaterialPath;

	std::unordered_map<unsigned, std::string> myTextureNames;
	std::unordered_map<unsigned, std::string> myTexturePaths;

	std::string myPSOName;
	std::string myPSOPath;
	std::unordered_map<ShaderType, std::string> myShaderNames;
	std::unordered_map<ShaderType, std::string> myShaderPaths;

	ImFont* newFont = nullptr;
	unsigned currentDebugMode = 0;
	unsigned currentTonemapper = 0;
	float currentBlendtime = 0.5f;

	std::vector<std::string> debugModeNames = {
	"None",
	"Albedo",
	"Ambient Occlusion",
	"Roughness",
	"Metallic",
	"Effects",
	"Wireframe",
	"Vertex Normals",
	"Vertex Tangents",
	"Vertex Binormals",
	"Pixel Normals",
	"Texture Normals",
	"UV0",
	"VertexColor"
	};

	std::vector<std::string> tonemapperNames = {
		"Unreal Engine",
		"ACES",
		"Lottes"
	};

	Math::Vector3f cameraStartingPos;
	Math::Vector3f cameraStartingRot;
	Math::Vector3f dLightStartingRot;
	float dLightStartIntensity = 0;
	float aLightStartIntensity = 0;
	bool switchToAnimationTab = false;

	std::string currentAnimName;
	std::string previousAnimName;
};
