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

#include "../Intermediate/Shaders/CompiledShaderHeaders/Default_VS.h"
#include "../Intermediate/Shaders/CompiledShaderHeaders/Default_PS.h"
#include "../Intermediate/Shaders/CompiledShaderHeaders/ShadowCube_VS.h"
#include "../Intermediate/Shaders/CompiledShaderHeaders/ShadowCube_GS.h"

#include "../Intermediate/Shaders/CompiledShaderHeaders/Sprite_VS.h"
#include "../Intermediate/Shaders/CompiledShaderHeaders/Sprite_GS.h"
#include "../Intermediate/Shaders/CompiledShaderHeaders/Sprite_PS.h"

#include "../Intermediate/Shaders/CompiledShaderHeaders/DebugLine_VS.h"
#include "../Intermediate/Shaders/CompiledShaderHeaders/DebugLine_GS.h"
#include "../Intermediate/Shaders/CompiledShaderHeaders/DebugObject_PS.h"

#include "../Intermediate/Shaders/CompiledShaderHeaders/Unlit_PS.h"
#include "../Intermediate/Shaders/CompiledShaderHeaders/Wireframe_PS.h"
#include "../Intermediate/Shaders/CompiledShaderHeaders/DebugVertexNormals_PS.h"
#include "../Intermediate/Shaders/CompiledShaderHeaders/DebugPixelNormals_PS.h"
#include "../Intermediate/Shaders/CompiledShaderHeaders/DebugTextureNormals_PS.h"
#include "../Intermediate/Shaders/CompiledShaderHeaders/DebugUVs_PS.h"
#include "../Intermediate/Shaders/CompiledShaderHeaders/Gizmo_PS.h"

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

	PipelineStateObject defaultPSO;
	defaultPSO.VertexShader = std::make_shared<Shader>();
	myRHI->LoadShaderFromMemory("Default_VS", *defaultPSO.VertexShader, BuiltIn_Default_VS_ByteCode, sizeof(BuiltIn_Default_VS_ByteCode));

	defaultPSO.PixelShader = std::make_shared<Shader>();
	myRHI->LoadShaderFromMemory("Default_PS", *defaultPSO.PixelShader, BuiltIn_Default_PS_ByteCode, sizeof(BuiltIn_Default_PS_ByteCode));

	myRHI->CreateInputLayout(defaultPSO.InputLayout, Vertex::InputLayoutDefinition, BuiltIn_Default_VS_ByteCode, sizeof(BuiltIn_Default_VS_ByteCode));
	
	defaultPSO.SamplerStates[0] = myRHI->GetSamplerState("DefaultSS");
	defaultPSO.SamplerStates[14] = myRHI->GetSamplerState("LutSS");
	defaultPSO.SamplerStates[15] = myRHI->GetSamplerState("ShadowSS");

	defaultPSO.VertexStride = sizeof(Vertex);
	myPSOmap.emplace(PipelineStateType::Default, std::make_shared<PipelineStateObject>(defaultPSO));


	PipelineStateObject spritePSO;
	spritePSO.VertexShader = std::make_shared<Shader>();
	myRHI->LoadShaderFromMemory("Sprite_VS", *spritePSO.VertexShader, BuiltIn_Sprite_VS_ByteCode, sizeof(BuiltIn_Sprite_VS_ByteCode));

	spritePSO.GeometryShader = std::make_shared<Shader>();
	myRHI->LoadShaderFromMemory("Sprite_GS", *spritePSO.GeometryShader, BuiltIn_Sprite_GS_ByteCode, sizeof(BuiltIn_Sprite_GS_ByteCode));

	spritePSO.PixelShader = std::make_shared<Shader>();
	myRHI->LoadShaderFromMemory("Sprite_PS", *spritePSO.PixelShader, BuiltIn_Sprite_PS_ByteCode, sizeof(BuiltIn_Sprite_PS_ByteCode));

	spritePSO.SamplerStates[0] = myRHI->GetSamplerState("DefaultSS");
	myPSOmap.emplace(PipelineStateType::Sprite, std::make_shared<PipelineStateObject>(spritePSO));


	PipelineStateObject debugPSO;
	myRHI->CreateInputLayout(debugPSO.InputLayout, DebugLineVertex::InputLayoutDefinition, BuiltIn_DebugLine_VS_ByteCode, sizeof(BuiltIn_DebugLine_VS_ByteCode));
	debugPSO.VertexStride = sizeof(DebugLineVertex);

	debugPSO.VertexShader = std::make_shared<Shader>();
	myRHI->LoadShaderFromMemory("DebugLine_VS", *debugPSO.VertexShader, BuiltIn_DebugLine_VS_ByteCode, sizeof(BuiltIn_DebugLine_VS_ByteCode));

	debugPSO.GeometryShader = std::make_shared<Shader>();
	myRHI->LoadShaderFromMemory("DebugLine_GS", *debugPSO.GeometryShader, BuiltIn_DebugLine_GS_ByteCode, sizeof(BuiltIn_DebugLine_GS_ByteCode));

	debugPSO.PixelShader = std::make_shared<Shader>();
	myRHI->LoadShaderFromMemory("DebugObject_PS", *debugPSO.PixelShader, BuiltIn_DebugObject_PS_ByteCode, sizeof(BuiltIn_DebugObject_PS_ByteCode));

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

	myRHI->CreateRasterizerState("DebugPSO_Rasterizer", debugRastDesc, debugPSO);

	debugPSO.SamplerStates[0] = myRHI->GetSamplerState("DefaultSS");
	myPSOmap.emplace(PipelineStateType::DebugLine, std::make_shared<PipelineStateObject>(debugPSO));

	
	PipelineStateObject unlitPSO;
	unlitPSO.SamplerStates[0] = myRHI->GetSamplerState("DefaultSS");
	unlitPSO.VertexShader = defaultPSO.VertexShader;
	unlitPSO.InputLayout = defaultPSO.InputLayout;
	unlitPSO.VertexStride = sizeof(Vertex);

	unlitPSO.PixelShader = std::make_shared<Shader>();
	if (!myRHI->LoadShaderFromMemory("Unlit_PS", *unlitPSO.PixelShader, BuiltIn_Unlit_PS_ByteCode, sizeof(BuiltIn_Unlit_PS_ByteCode)))
	{
		LOG(GraphicsLog, Error, "Failed to load pixel shader!");
		return false;
	}
	myPSOmap.emplace(PipelineStateType::Unlit, std::make_shared<PipelineStateObject>(unlitPSO));

	
	PipelineStateObject wireframePSO;
	wireframePSO = unlitPSO;
	D3D11_RASTERIZER_DESC wireframeRastDesc = {};
	wireframeRastDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeRastDesc.CullMode = D3D11_CULL_NONE;
	myRHI->CreateRasterizerState("WireframePSO_Rasterizer", wireframeRastDesc, wireframePSO);

	wireframePSO.PixelShader = std::make_shared<Shader>();
	myRHI->LoadShaderFromMemory("Wireframe_PS", *wireframePSO.PixelShader, BuiltIn_Wireframe_PS_ByteCode, sizeof(BuiltIn_Wireframe_PS_ByteCode));
	myPSOmap.emplace(PipelineStateType::Wireframe, std::make_shared<PipelineStateObject>(wireframePSO));



	PipelineStateObject gizmoPSO;
	gizmoPSO.VertexShader = defaultPSO.VertexShader;

	gizmoPSO.PixelShader = std::make_shared<Shader>();
	myRHI->LoadShaderFromMemory("Gizmo_PS", *gizmoPSO.PixelShader, BuiltIn_Gizmo_PS_ByteCode, sizeof(BuiltIn_Gizmo_PS_ByteCode));

	gizmoPSO.InputLayout = defaultPSO.InputLayout;
	gizmoPSO.VertexStride = sizeof(Vertex);

	myPSOmap.emplace(PipelineStateType::Gizmo, std::make_shared<PipelineStateObject>(gizmoPSO));
	

	PipelineStateObject shadowPSO;
	shadowPSO.VertexShader = defaultPSO.VertexShader;
	shadowPSO.InputLayout = defaultPSO.InputLayout;
	shadowPSO.VertexStride = sizeof(Vertex);
	myPSOmap.emplace(PipelineStateType::Shadow, std::make_shared<PipelineStateObject>(shadowPSO));
	
	PipelineStateObject shadowCubePSO;
	shadowCubePSO.VertexShader = std::make_shared<Shader>();
	myRHI->LoadShaderFromMemory("ShadowCube_VS", *shadowCubePSO.VertexShader, BuiltIn_ShadowCube_VS_ByteCode, sizeof(BuiltIn_ShadowCube_VS_ByteCode));

	shadowCubePSO.GeometryShader = std::make_shared<Shader>();
	myRHI->LoadShaderFromMemory("ShadowCube_GS", *shadowCubePSO.GeometryShader, BuiltIn_ShadowCube_GS_ByteCode, sizeof(BuiltIn_ShadowCube_GS_ByteCode));

	shadowCubePSO.InputLayout = defaultPSO.InputLayout;
	shadowCubePSO.VertexStride = sizeof(Vertex);
	myPSOmap.emplace(PipelineStateType::ShadowCube, std::make_shared<PipelineStateObject>(shadowCubePSO));

	PipelineStateObject debugVertexNormalsPSO;
	debugVertexNormalsPSO = unlitPSO;
	debugVertexNormalsPSO.PixelShader = std::make_shared<Shader>();
	myRHI->LoadShaderFromMemory("DebugVertexNormals_PS", *debugVertexNormalsPSO.PixelShader, BuiltIn_DebugVertexNormals_PS_ByteCode, sizeof(BuiltIn_DebugVertexNormals_PS_ByteCode));
	myPSOmap.emplace(PipelineStateType::DebugVertexNormals, std::make_shared<PipelineStateObject>(debugVertexNormalsPSO));

	PipelineStateObject debugPixelNormalsPSO;
	debugPixelNormalsPSO = unlitPSO;
	debugPixelNormalsPSO.PixelShader = std::make_shared<Shader>();
	myRHI->LoadShaderFromMemory("DebugPixelNormals_PS", *debugPixelNormalsPSO.PixelShader, BuiltIn_DebugPixelNormals_PS_ByteCode, sizeof(BuiltIn_DebugPixelNormals_PS_ByteCode));
	myPSOmap.emplace(PipelineStateType::DebugPixelNormals, std::make_shared<PipelineStateObject>(debugPixelNormalsPSO));

	PipelineStateObject debugTextureNormalsPSO;
	debugTextureNormalsPSO = unlitPSO;
	debugTextureNormalsPSO.PixelShader = std::make_shared<Shader>();
	myRHI->LoadShaderFromMemory("DebugPixelNormals_PS", *debugTextureNormalsPSO.PixelShader, BuiltIn_DebugTextureNormals_PS_ByteCode, sizeof(BuiltIn_DebugTextureNormals_PS_ByteCode));
	myPSOmap.emplace(PipelineStateType::DebugTextureNormals, std::make_shared<PipelineStateObject>(debugTextureNormalsPSO));

	PipelineStateObject debugUVsPSO;
	debugUVsPSO = unlitPSO;
	debugUVsPSO.PixelShader = std::make_shared<Shader>();
	myRHI->LoadShaderFromMemory("DebugUVs_PS", *debugUVsPSO.PixelShader, BuiltIn_DebugUVs_PS_ByteCode, sizeof(BuiltIn_DebugUVs_PS_ByteCode));
	myPSOmap.emplace(PipelineStateType::DebugUVs, std::make_shared<PipelineStateObject>(debugUVsPSO));


	myLUTtexture = std::make_shared<Texture>();
	myRHI->CreateLUT("LUT", 512, 512, myLUTtexture);

	CreateConstantBuffers();
	
	myCurrentPSO = myPSOmap[PipelineStateType::Default];
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
