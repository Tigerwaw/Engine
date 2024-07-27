#include "GraphicsEngine.pch.h"
#include "GraphicsEngine.h"

#include "Logger\Logger.h"

#include "GraphicsEngine/Objects/Shader.h"
#include "GraphicsEngine/Objects/Vertex.h"
#include "GraphicsEngine/Objects/DebugLineVertex.h"
#include "GraphicsEngine/Objects/Mesh.h"
#include "GraphicsEngine/Objects/Sprite.h"
#include "GraphicsEngine/Objects/Material.h"
#include "GraphicsEngine/Objects/Texture.h"
#include "GraphicsEngine/Objects/ConstantBuffers/FrameBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/ObjectBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/AnimationBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/MaterialBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/LightBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/ShadowBuffer.h"
#include "GraphicsEngine/Objects/ConstantBuffers/SpriteBuffer.h"

#include "GraphicsEngine/Objects/DynamicVertexBuffer.h"

// TEMP
#include "AssetManager/AssetManager.h"

#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(GraphicsLog, GraphicsEngine, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(GraphicsLog, GraphicsEngine, Error);
#endif

DEFINE_LOG_CATEGORY(GraphicsLog);

GraphicsEngine& GraphicsEngine::Get()
{
	static GraphicsEngine myInstance;
	return myInstance;
}

bool GraphicsEngine::Initialize(HWND aWindowHandle)
{
	myRHI = std::make_unique<RenderHardwareInterface>();

	LOG(GraphicsLog, Log, "Initializing Graphics Engine...");

	if (!myRHI->Initialize(aWindowHandle, true))
	{
		myRHI.reset();
		LOG(GraphicsLog, Error, "Failed to initialize graphics engine!");
		return false;
	}

	{
		PipelineStateObject defaultPSO;
		defaultPSO.SamplerStates[0] = myRHI->GetSamplerState("DefaultSS");

		defaultPSO.VertexStride = sizeof(Vertex);

		if (!myRHI->CreateInputLayout(defaultPSO.InputLayout, Vertex::InputLayoutDefinition, L"../../Assets/Shaders/Mesh_VS.cso"))
		{
			LOG(GraphicsLog, Error, "Failed to load default input layout!");
			return false;
		}

		defaultPSO.VertexShader = std::make_shared<Shader>();
		if (!myRHI->LoadShaderFromFilePath("Default_VS", *defaultPSO.VertexShader, L"../../Assets/Shaders/Mesh_VS.cso"))
		{
			LOG(GraphicsLog, Error, "Failed to load default vertex shader!");
			return false;
		}

		defaultPSO.PixelShader = std::make_shared<Shader>();
		if (!myRHI->LoadShaderFromFilePath("Default_PS", *defaultPSO.PixelShader, L"../../Assets/Shaders/Unlit_PS.cso"))
		{
			LOG(GraphicsLog, Error, "Failed to load default pixel shader!");
			return false;
		}

		myPSOmap.emplace(PipelineStateType::Default, std::make_shared<PipelineStateObject>(defaultPSO));

		myCurrentPSO = myPSOmap[PipelineStateType::Default];
	}

	myLUTtexture = std::make_shared<Texture>();
	myRHI->CreateLUT("LUT", 512, 512, myLUTtexture);

	CreateConstantBuffers();
	
	myCommandList = std::make_unique<GraphicsCommandList>();

	LOG(GraphicsLog, Log, "Initialized Graphics Engine!");
	return true;
}

#ifdef _DEBUG
bool GraphicsEngine::InitializeImGui()
{
	return myRHI->InitializeImGui();
}
#endif

bool GraphicsEngine::InitializePSOs()
{
	std::unordered_map<unsigned, std::string> samplers;
	samplers.emplace(0, "DefaultSS");
	samplers.emplace(14, "LutSS");
	samplers.emplace(15, "ShadowSS");
	CreatePSO("PBR", PipelineStateType::PBR, Vertex::InputLayoutDefinition, sizeof(Vertex), L"../../Assets/Shaders/Mesh_VS.cso",
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/Mesh_VS.cso")->shader, 
		nullptr, 
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/PBRMesh_PS.cso")->shader,
		nullptr, &samplers);

	samplers.clear();
	samplers.emplace(0, "DefaultSS");
	CreatePSO("Sprite", PipelineStateType::Sprite, Vertex::InputLayoutDefinition, sizeof(Vertex), L"../../Assets/Shaders/Sprite_VS.cso",
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/Sprite_VS.cso")->shader,
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/Sprite_GS.cso")->shader,
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/Sprite_PS.cso")->shader,
		nullptr, &samplers);

	D3D11_RASTERIZER_DESC debugRastDesc = {};
	debugRastDesc.FillMode = D3D11_FILL_SOLID;
	debugRastDesc.CullMode = D3D11_CULL_NONE;
	debugRastDesc.FrontCounterClockwise = false;
	debugRastDesc.DepthBias = 0;
	debugRastDesc.SlopeScaledDepthBias = 0;
	debugRastDesc.DepthBiasClamp = 0;
	debugRastDesc.DepthClipEnable = true;
	debugRastDesc.ScissorEnable = false;
	debugRastDesc.AntialiasedLineEnable = true;
	debugRastDesc.MultisampleEnable = false;
	CreatePSO("DebugLine", PipelineStateType::DebugLine, DebugLineVertex::InputLayoutDefinition, sizeof(DebugLineVertex), L"../../Assets/Shaders/DebugLine_VS.cso",
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/DebugLine_VS.cso")->shader,
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/DebugLine_GS.cso")->shader,
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/DebugObject_PS.cso")->shader,
		&debugRastDesc, nullptr);


	samplers.clear();
	samplers.emplace(0, "DefaultSS");
	CreatePSO("Unlit", PipelineStateType::Unlit, Vertex::InputLayoutDefinition, sizeof(Vertex), L"../../Assets/Shaders/Mesh_VS.cso",
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/Mesh_VS.cso")->shader,
		nullptr,
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/Unlit_PS.cso")->shader,
		nullptr, &samplers);

	D3D11_RASTERIZER_DESC wireframeRastDesc = {};
	wireframeRastDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeRastDesc.CullMode = D3D11_CULL_NONE;
	CreatePSO("Wireframe", PipelineStateType::Wireframe, Vertex::InputLayoutDefinition, sizeof(Vertex), L"../../Assets/Shaders/Mesh_VS.cso",
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/Mesh_VS.cso")->shader,
		nullptr,
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/Wireframe_PS.cso")->shader,
		&wireframeRastDesc, nullptr);

	CreatePSO("Gizmo", PipelineStateType::Gizmo, Vertex::InputLayoutDefinition, sizeof(Vertex), L"../../Assets/Shaders/Mesh_VS.cso",
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/Mesh_VS.cso")->shader,
		nullptr,
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/Gizmo_PS.cso")->shader,
		nullptr, nullptr);

	CreatePSO("Shadow", PipelineStateType::Shadow, Vertex::InputLayoutDefinition, sizeof(Vertex), L"../../Assets/Shaders/Mesh_VS.cso",
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/Mesh_VS.cso")->shader,
		nullptr,
		nullptr,
		nullptr, nullptr);

	CreatePSO("ShadowCube", PipelineStateType::ShadowCube, Vertex::InputLayoutDefinition, sizeof(Vertex), L"../../Assets/Shaders/ShadowCube_VS.cso",
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/ShadowCube_VS.cso")->shader,
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/ShadowCube_GS.cso")->shader,
		nullptr,
		nullptr, nullptr);

	CreatePSO("DebugVertexNormals", PipelineStateType::DebugVertexNormals, Vertex::InputLayoutDefinition, sizeof(Vertex), L"../../Assets/Shaders/Mesh_VS.cso",
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/Mesh_VS.cso")->shader,
		nullptr,
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/DebugVertexNormals_PS.cso")->shader,
		nullptr, nullptr);

	samplers.clear();
	samplers.emplace(0, "DefaultSS");
	CreatePSO("DebugPixelNormals", PipelineStateType::DebugPixelNormals, Vertex::InputLayoutDefinition, sizeof(Vertex), L"../../Assets/Shaders/Mesh_VS.cso",
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/Mesh_VS.cso")->shader,
		nullptr,
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/DebugPixelNormals_PS.cso")->shader,
		nullptr, &samplers);

	samplers.clear();
	samplers.emplace(0, "DefaultSS");
	CreatePSO("DebugTextureNormals", PipelineStateType::DebugTextureNormals, Vertex::InputLayoutDefinition, sizeof(Vertex), L"../../Assets/Shaders/Mesh_VS.cso",
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/Mesh_VS.cso")->shader,
		nullptr,
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/DebugTextureNormals_PS.cso")->shader,
		nullptr, &samplers);

	CreatePSO("DebugUVs", PipelineStateType::DebugUVs, Vertex::InputLayoutDefinition, sizeof(Vertex), L"../../Assets/Shaders/Mesh_VS.cso",
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/Mesh_VS.cso")->shader,
		nullptr,
		AssetManager::Get().GetAsset<ShaderAsset>("Shaders/DebugUVs_PS.cso")->shader,
		nullptr, nullptr);

	return true;
}

void GraphicsEngine::BeginFrame()
{
	myDrawcallAmount = 0;
	ChangePipelineState(myCurrentPSO);
}

void GraphicsEngine::RenderFrame()
{
	if (myCommandList->HasCommands() && !myCommandList->IsFinished())
	{
		myCommandList->Execute();
	}
	myCommandList->Reset();
}

void GraphicsEngine::EndFrame()
{
	myRHI->Present();
}

void GraphicsEngine::ChangePipelineState(PipelineStateType aPipelineState)
{
	if (!myPSOmap[aPipelineState])
	{
		LOG(GraphicsLog, Error, "PSO {} does not exist!", static_cast<int>(aPipelineState));
		return;
	}

	ChangePipelineState(myPSOmap[aPipelineState]);
}

void GraphicsEngine::ChangePipelineState(const std::shared_ptr<PipelineStateObject> aNewPSO)
{
	myRHI->ChangePipelineState(*aNewPSO);
	myCurrentPSO = aNewPSO;
}

std::shared_ptr<PipelineStateObject> GraphicsEngine::GetPSO(PipelineStateType aPipelineState)
{
	return myPSOmap[aPipelineState];
}

bool GraphicsEngine::LoadTexture(std::string_view aName, const uint8_t* aTextureDataPtr, size_t aTextureDataSize, Texture& outTexture) const
{
	return myRHI->LoadTexture(aName, aTextureDataPtr, aTextureDataSize, outTexture);
}

bool GraphicsEngine::LoadTexture(std::filesystem::path& aFilePath, Texture& outTexture) const
{
	std::ifstream file(aFilePath, std::ios::binary);
	file.seekg(0, std::ios::end);
	const std::streamoff fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<uint8_t> fileData;
	fileData.resize(fileSize);
	file.read(reinterpret_cast<char*>(fileData.data()), fileSize);
	file.close();
	
	return myRHI->LoadTexture(aFilePath.filename().string(), fileData.data(), static_cast<size_t>(fileSize), outTexture);
}

bool GraphicsEngine::SetTextureResource_PS(unsigned aSlot, Texture& aTexture)
{
	return myRHI->SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, aSlot, aTexture);
}

bool GraphicsEngine::SetTextureResource_VS(unsigned aSlot, Texture& aTexture)
{
	return myRHI->SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER, aSlot, aTexture);
}

bool GraphicsEngine::ClearTextureResource_PS(unsigned aSlot)
{
	return myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, aSlot);
}

bool GraphicsEngine::ClearTextureResource_VS(unsigned aSlot)
{
	return myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_VERTEX_SHADER, aSlot);
}

bool GraphicsEngine::LoadShader(std::filesystem::path aFilePath, Shader& outShader)
{
	return myRHI->LoadShaderFromFilePath(aFilePath.stem().string(), outShader, aFilePath.generic_wstring());
}

bool GraphicsEngine::CreatePSO(std::string aName, PipelineStateType aType,
							   std::vector<VertexElementDesc> aInputLayoutDefinition, unsigned aVertexStride, std::wstring aVSpath,
							   std::shared_ptr<Shader> aVSshader, std::shared_ptr<Shader> aGSshader, std::shared_ptr<Shader> aPSshader,
							   D3D11_RASTERIZER_DESC* aRasterizerDesc, std::unordered_map<unsigned, std::string>* aSamplerList)
{
	std::shared_ptr<PipelineStateObject> newPSO = std::make_shared<PipelineStateObject>();
	newPSO->VertexStride = aVertexStride;

	if (aVSpath != L"")
	{
		if (!myRHI->CreateInputLayout(newPSO->InputLayout, aInputLayoutDefinition, aVSpath))
		{
			LOG(GraphicsLog, Error, "Failed to create PSO!");
			return false;
		}
	}

	newPSO->VertexShader = aVSshader;
	newPSO->GeometryShader = aGSshader;
	newPSO->PixelShader = aPSshader;

	if (aRasterizerDesc)
	{
		if (!myRHI->CreateRasterizerState(aName + "_Rasterizer", *aRasterizerDesc, *newPSO))
		{
			LOG(GraphicsLog, Error, "Failed to create PSO!");
			return false;
		}
	}

	if (aSamplerList)
	{
		for (auto& sampler : *aSamplerList)
		{
			newPSO->SamplerStates[sampler.first] = myRHI->GetSamplerState(sampler.second);
		}
	}

	myPSOmap.emplace(aType, newPSO);
	LOG(GraphicsLog, Log, "Created PSO {}!", aName);
	return true;
}

bool GraphicsEngine::CreateShadowMap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture)
{
	return myRHI->CreateShadowMap(aName, aWidth, aHeight, outTexture);
}

bool GraphicsEngine::CreateShadowCubemap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture)
{
	return myRHI->CreateShadowCubemap(aName, aWidth, aHeight, outTexture);
}

void GraphicsEngine::SetRenderTarget(std::shared_ptr<Texture> aRenderTarget, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget, bool aClearDepthStencil)
{
	myRHI->SetRenderTarget(aRenderTarget, aDepthStencil, aClearRenderTarget, aClearDepthStencil);
}

void GraphicsEngine::RenderMesh(const Mesh& aMesh, std::vector<std::shared_ptr<Material>> aMaterialList)
{
	myRHI->SetVertexBuffer(aMesh.GetVertexBuffer(), myCurrentPSO->VertexStride, 0);
	myRHI->SetIndexBuffer(aMesh.GetIndexBuffer());
	myRHI->SetPrimitiveTopology(Topology::TRIANGLELIST);

	SetTextureResource_PS(127, *myLUTtexture);

	for (const auto& element : aMesh.GetElements())
	{
		if (aMaterialList.size() > element.MaterialIndex)
		{
			// Surely there is a better way
			if (myCurrentPSO != GetPSO(PipelineStateType::Shadow) && myCurrentPSO != GetPSO(PipelineStateType::ShadowCube))
			{
				MaterialBuffer matBufferData = aMaterialList[element.MaterialIndex]->MaterialSettings();
				GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer, matBufferData);

				switch (myCurrentDebugMode)
				{
				case DebugMode::None:
				{
					if (aMaterialList[element.MaterialIndex]->GetPSO())
					{
						ChangePipelineState(aMaterialList[element.MaterialIndex]->GetPSO());
					}
					break;
				}
				case DebugMode::Unlit:
					ChangePipelineState(PipelineStateType::Unlit);
					break;
				case DebugMode::Wireframe:
					ChangePipelineState(PipelineStateType::Wireframe);
					break;
				case DebugMode::DebugVertexNormals:
					ChangePipelineState(PipelineStateType::DebugVertexNormals);
					break;
				case DebugMode::DebugPixelNormals:
					ChangePipelineState(PipelineStateType::DebugPixelNormals);
					break;
				case DebugMode::DebugTextureNormals:
					ChangePipelineState(PipelineStateType::DebugTextureNormals);
					break;
				case DebugMode::DebugUVs:
					ChangePipelineState(PipelineStateType::DebugUVs);
					break;
				default:
					break;
				}

				SetTextureResource_PS(0, aMaterialList[element.MaterialIndex]->GetAlbedoTexture());
				SetTextureResource_PS(1, aMaterialList[element.MaterialIndex]->GetNormalTexture());
				SetTextureResource_PS(2, aMaterialList[element.MaterialIndex]->GetMaterialTexture());
			}
		}

		myRHI->DrawIndexed(element.IndexOffset, element.NumIndices);
		myDrawcallAmount++;
	}

	ClearTextureResource_PS(0);
	ClearTextureResource_PS(1);
	ClearTextureResource_PS(2);
	ClearTextureResource_PS(127);
}

void GraphicsEngine::RenderSprite()
{
	ChangePipelineState(PipelineStateType::Sprite);
	myRHI->SetPrimitiveTopology(Topology::POINTLIST);
	myRHI->Draw(1);
}

void GraphicsEngine::RenderDebugLines(DynamicVertexBuffer& aDynamicBuffer, unsigned aLineAmount)
{
	ChangePipelineState(PipelineStateType::DebugLine);
	myRHI->SetVertexBuffer(aDynamicBuffer.GetVertexBuffer(), myCurrentPSO->VertexStride, 0);
	myRHI->SetPrimitiveTopology(Topology::POINTLIST);
	myRHI->Draw(aLineAmount);
}

bool GraphicsEngine::CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer)
{
	return myRHI->CreateIndexBuffer(aName, aIndexList, outIxBuffer);
}

GraphicsEngine::GraphicsEngine() = default;
GraphicsEngine::~GraphicsEngine() = default;

void GraphicsEngine::CreateConstantBuffers()
{
	ConstantBuffer frameBuffer;
	myRHI->CreateConstantBuffer("FrameBuffer", sizeof(FrameBuffer), 0, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_GEOMETRY_SHADER | PIPELINE_STAGE_PIXEL_SHADER, frameBuffer);
	myConstantBuffers.emplace(ConstantBufferType::FrameBuffer, std::move(frameBuffer));

	ConstantBuffer objectBuffer;
	myRHI->CreateConstantBuffer("ObjectBuffer", sizeof(ObjectBuffer), 1, PIPELINE_STAGE_VERTEX_SHADER, objectBuffer);
	myConstantBuffers.emplace(ConstantBufferType::ObjectBuffer, std::move(objectBuffer));

	ConstantBuffer animationBuffer;
	myRHI->CreateConstantBuffer("AnimationBuffer", sizeof(AnimationBuffer), 2, PIPELINE_STAGE_VERTEX_SHADER, animationBuffer);
	myConstantBuffers.emplace(ConstantBufferType::AnimationBuffer, std::move(animationBuffer));

	ConstantBuffer materialBuffer;
	myRHI->CreateConstantBuffer("MaterialBuffer", sizeof(MaterialBuffer), 3, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER, materialBuffer);
	myConstantBuffers.emplace(ConstantBufferType::MaterialBuffer, std::move(materialBuffer));

	ConstantBuffer lightBuffer;
	myRHI->CreateConstantBuffer("LightBuffer", sizeof(LightBuffer), 4, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER, lightBuffer);
	myConstantBuffers.emplace(ConstantBufferType::LightBuffer, std::move(lightBuffer));

	ConstantBuffer shadowBuffer;
	myRHI->CreateConstantBuffer("ShadowBuffer", sizeof(ShadowBuffer), 5, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_GEOMETRY_SHADER | PIPELINE_STAGE_PIXEL_SHADER, shadowBuffer);
	myConstantBuffers.emplace(ConstantBufferType::ShadowBuffer, std::move(shadowBuffer));

	ConstantBuffer spriteBuffer;
	myRHI->CreateConstantBuffer("SpriteBuffer", sizeof(SpriteBuffer), 6, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_GEOMETRY_SHADER | PIPELINE_STAGE_PIXEL_SHADER, spriteBuffer);
	myConstantBuffers.emplace(ConstantBufferType::SpriteBuffer, std::move(spriteBuffer));
}
