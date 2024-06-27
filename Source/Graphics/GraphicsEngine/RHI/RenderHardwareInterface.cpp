#include "GraphicsEngine.pch.h"
#include "RenderHardwareInterface.h"
#include <d3d11_1.h>

#include <d3dcompiler.h>
#include <d3d11shader.h>
#include "GraphicsEngine/DDSTextureLoader/DDSTextureLoader11.h"
#include "Logger\Logger.h"
#include "StringHelpers.h"

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#include "Texture.h"
#include "Shader.h"
#include "GraphicsEngine/Objects/ConstantBuffers/ConstantBuffer.h"
#include "PipelineStateObject.h"

#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(RhiLog, "RHI", Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(RhiLog, "RHI", Error);
#endif

DEFINE_LOG_CATEGORY(RhiLog);

using namespace Microsoft::WRL;

RenderHardwareInterface::RenderHardwareInterface() = default;
RenderHardwareInterface::~RenderHardwareInterface() = default;

bool RenderHardwareInterface::Initialize(HWND aWindowHandle, bool aEnableDebug)
{
	HRESULT result = E_FAIL;

	ComPtr<IDXGIFactory> dxFactory;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), &dxFactory);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create DX Factory!");
		return false;
	}

	LOG(RhiLog, Log, "Initializing RHI...");

	ComPtr<IDXGIAdapter> tempAdapter;
	std::vector<ComPtr<IDXGIAdapter>> adapters;
	unsigned adapterCount = 0;
	while (dxFactory->EnumAdapters(adapterCount, &tempAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		adapters.push_back(tempAdapter);
		adapterCount++;
	}

	ComPtr<IDXGIAdapter> selectedAdapter;
	size_t selectedAdapterVRAM = 0;
	DXGI_ADAPTER_DESC selectedAdapterDesc = {};
	for (const auto& adapter : adapters)
	{
		DXGI_ADAPTER_DESC currentDesc = {};
		adapter->GetDesc(&currentDesc);

		if (currentDesc.DedicatedVideoMemory > selectedAdapterVRAM)
		{
			selectedAdapterVRAM = currentDesc.DedicatedVideoMemory;
			selectedAdapter = adapter;
			selectedAdapterDesc = currentDesc;
		}
	}

	const wchar_t* wideAdapterName = selectedAdapterDesc.Description;
	const std::string adapterName = str::wide_to_utf8(wideAdapterName);

	LOG(RhiLog, Log, "Selected adapter is {}", adapterName);
	constexpr size_t megabyte = (1024ULL * 1024ULL);
	if (selectedAdapterVRAM > megabyte)
	{
		selectedAdapterVRAM /= megabyte;
	}

	LOG(RhiLog, Log, " * VRAM: {} MB", selectedAdapterVRAM);

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	result = D3D11CreateDevice(
		selectedAdapter.Get(),
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		aEnableDebug ? D3D11_CREATE_DEVICE_DEBUG : 0,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&device,
		NULL,
		&context
	);

	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to initialize DirectX!");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32-bit unsigned normalized
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = aWindowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	swapChainDesc.Windowed = true;

	ComPtr<IDXGISwapChain> swapChain;
	result = dxFactory->CreateSwapChain(device.Get(), &swapChainDesc, &swapChain);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create SwapChain!");
		return false;
	}

	ComPtr<ID3D11Texture2D> backBufferTexture;
	result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBufferTexture);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to retrieve back buffer!");
		return false;
	}

	myBackBuffer = std::make_shared<Texture>();
	result = device->CreateRenderTargetView(backBufferTexture.Get(), nullptr, myBackBuffer->myRTV.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create Render Target View!");
		return false;
	}

	RECT clientRect = {};
	GetClientRect(aWindowHandle, &clientRect);
	const float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
	const float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);
	myBackBuffer->myViewport = { 0, 0, clientWidth, clientHeight, 0, 1 };

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = myBackBuffer->myViewport[2];
	viewport.Height = myBackBuffer->myViewport[3];
	viewport.MinDepth = myBackBuffer->myViewport[4];
	viewport.MaxDepth = myBackBuffer->myViewport[5];
	context->RSSetViewports(1, &viewport);

	myDepthBuffer = std::make_shared<Texture>();

	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = static_cast<unsigned>(clientWidth);
	depthDesc.Height = static_cast<unsigned>(clientHeight);
	depthDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;

	ComPtr<ID3D11Texture2D> depthTexture;
	result = device->CreateTexture2D(&depthDesc, nullptr, depthTexture.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create depth buffer!");
		return false;
	}
	
	SetObjectName(depthTexture, "DepthBuffer_T2D");

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	result = device->CreateDepthStencilView(depthTexture.Get(), &dsvDesc, myDepthBuffer->myDSV.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create depth stencil view!");
		return false;
	}

	SetObjectName(myDepthBuffer->myDSV, "DepthBuffer_DSV");

	// Query if we support ID3DUserDefinedAnnotation.
	context->QueryInterface(IID_PPV_ARGS(&myUDA));

	myDevice = device;
	myContext = context;
	mySwapChain = swapChain;

	SetObjectName(myContext, "Device Context");
	SetObjectName(myBackBuffer->myRTV, "BackBuffer RTV");

	CreateDefaultSamplerStates();

	// Initialize imgui dx11
	{
		ImGui_ImplDX11_Init(myDevice.Get(), myContext.Get());
	}

	LOG(RhiLog, Log, "RHI Initialized!");

	return true;
}

void RenderHardwareInterface::Present() const
{
	mySwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
}

bool RenderHardwareInterface::CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer)
{
	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.ByteWidth = static_cast<unsigned>(aIndexList.size() * sizeof(unsigned));
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexSubresourceData = {};
	indexSubresourceData.pSysMem = aIndexList.data();

	const HRESULT result = myDevice->CreateBuffer(&indexBufferDesc, &indexSubresourceData, outIxBuffer.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create Index Buffer - {}!", aName);
		return false;
	}

	SetObjectName(outIxBuffer, aName);
	return true;
}

bool RenderHardwareInterface::CreateConstantBuffer(std::string_view aName, size_t aSize, unsigned aSlot, unsigned aPipelineStages, ConstantBuffer& outBuffer)
{
	if (aSize > 65536)
	{
		LOG(RhiLog, Error, "Failed to create constant buffer {}. Size is larger than 64Kb ({}).", aName, aSize);
		return false;
	}

	outBuffer.myName = aName;
	outBuffer.mySize = aSize;
	outBuffer.myPipelineStages = aPipelineStages;
	outBuffer.mySlotIndex = aSlot;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth = static_cast<unsigned>(aSize);

	const HRESULT result = myDevice->CreateBuffer(&bufferDesc, nullptr, outBuffer.myBuffer.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create constant buffer {}. Check DirectX log for more information", aName);
		return false;
	}

	SetObjectName(outBuffer.myBuffer, aName);
	LOG(RhiLog, Log, "Created constant buffer {}.", aName);

	return true;
}

void RenderHardwareInterface::SetVertexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aVertexBuffer, size_t aVertexSize, size_t aVertexOffset) const
{
	const unsigned vertexSize = static_cast<unsigned>(aVertexSize);
	const unsigned vertexOffset = static_cast<unsigned>(aVertexOffset);
	myContext->IASetVertexBuffers(0, 1, aVertexBuffer.GetAddressOf(), &vertexSize, &vertexOffset);
}

void RenderHardwareInterface::SetIndexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aIndexBuffer) const
{
	DXGI_FORMAT indexBufferFormat = DXGI_FORMAT_R32_UINT;
	myContext->IASetIndexBuffer(aIndexBuffer.Get(), indexBufferFormat, 0);
}

void RenderHardwareInterface::SetConstantBuffer(const ConstantBuffer& aBuffer)
{
	if (aBuffer.myPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
	{
		myContext->VSSetConstantBuffers(aBuffer.mySlotIndex, 1, aBuffer.myBuffer.GetAddressOf());
	}

	if (aBuffer.myPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
	{
		myContext->PSSetConstantBuffers(aBuffer.mySlotIndex, 1, aBuffer.myBuffer.GetAddressOf());
	}

	if (aBuffer.myPipelineStages & PIPELINE_STAGE_GEOMETRY_SHADER)
	{
		myContext->GSSetConstantBuffers(aBuffer.mySlotIndex, 1, aBuffer.myBuffer.GetAddressOf());
	}
}

void RenderHardwareInterface::SetPrimitiveTopology(Topology aTopology) const
{
	myContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(aTopology));
}

bool RenderHardwareInterface::CreateInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout>& outInputLayout, 
												const std::vector<VertexElementDesc>& aInputLayoutDefinition, const uint8_t* aShaderDataPtr, size_t aShaderDataSize)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
	inputElements.reserve(aInputLayoutDefinition.size());

	for (const auto& vxED : aInputLayoutDefinition)
	{
		D3D11_INPUT_ELEMENT_DESC element = {};
		element.SemanticName = vxED.Semantic.data();
		element.SemanticIndex = vxED.SemanticIndex;
		element.Format = static_cast<DXGI_FORMAT>(vxED.Type);

		element.InputSlot = 0;
		element.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		element.InstanceDataStepRate = 0;

		inputElements.emplace_back(element);
	}

	const HRESULT result = myDevice->CreateInputLayout(
		inputElements.data(),
		static_cast<unsigned>(inputElements.size()),
		aShaderDataPtr,
		aShaderDataSize,
		outInputLayout.GetAddressOf()
	);

	SetObjectName(outInputLayout, "Default Input Layout");

	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create Input Layout!");
		return false;
	}

	return true;
}

void RenderHardwareInterface::SetInputLayout(const Microsoft::WRL::ComPtr<ID3D11InputLayout>& aInputLayout)
{
	myContext->IASetInputLayout(aInputLayout.Get());
}

bool RenderHardwareInterface::LoadShaderFromMemory(std::string_view aName, Shader& outShader, const uint8_t* aShaderDataPtr, size_t aShaderDataSize)
{
	ComPtr<ID3D11ShaderReflection> shaderReflection;
	HRESULT result = D3DReflect(
		aShaderDataPtr,
		aShaderDataSize,
		IID_ID3D11ShaderReflection,
		reinterpret_cast<void**>(shaderReflection.GetAddressOf())
	);

	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to load shader!");
		return false;
	}

	D3D11_SHADER_DESC shaderDesc = {};
	shaderReflection->GetDesc(&shaderDesc);

	D3D11_SHADER_VERSION_TYPE shaderVersion = static_cast<D3D11_SHADER_VERSION_TYPE>(D3D11_SHVER_GET_TYPE(shaderDesc.Version));

	switch (shaderVersion)
	{
	case D3D11_SHVER_VERTEX_SHADER:
	{
		outShader.myType = ShaderType::VertexShader;
		ComPtr<ID3D11VertexShader> vsShader;
		result = myDevice->CreateVertexShader(aShaderDataPtr, aShaderDataSize, nullptr, &vsShader);
		outShader.myShader = vsShader;
		break;
	}
	case D3D11_SHVER_PIXEL_SHADER:
	{
		outShader.myType = ShaderType::PixelShader;
		ComPtr<ID3D11PixelShader> psShader;
		result = myDevice->CreatePixelShader(aShaderDataPtr, aShaderDataSize, nullptr, &psShader);
		outShader.myShader = psShader;
		break;
	}
	case D3D11_SHVER_GEOMETRY_SHADER:
	{
		outShader.myType = ShaderType::GeometryShader;
		ComPtr<ID3D11GeometryShader> gsShader;
		result = myDevice->CreateGeometryShader(aShaderDataPtr, aShaderDataSize, nullptr, &gsShader);
		outShader.myShader = gsShader;
		break;
	}
	}

	SetObjectName(outShader.myShader, aName);

	return true;
}

void RenderHardwareInterface::Draw(unsigned aVertexCount)
{
	myContext->Draw(aVertexCount, 0);
}

void RenderHardwareInterface::DrawIndexed(unsigned aStartIndex, unsigned aIndexCount) const
{
	myContext->DrawIndexed(aIndexCount, aStartIndex, 0);
}

void RenderHardwareInterface::ChangePipelineState(const PipelineStateObject& aNewPSO, const PipelineStateObject& aOldPSO)
{
	if (aOldPSO.RenderTarget)
	{
		ID3D11RenderTargetView* nullRTV = nullptr;
		myContext->OMSetRenderTargets(1, &nullRTV, nullptr);
	}
	else
	{
		if (aOldPSO.DepthStencil)
		{
			myContext->OMSetRenderTargets(0, nullptr, nullptr);
		}
	}

	const std::array<float, 4> blendFactor = { 0, 0, 0, 0 };
	constexpr unsigned samplerMask = 0xffffffff;
	myContext->OMSetBlendState(aNewPSO.BlendState.Get(), blendFactor.data(), samplerMask);
	myContext->RSSetState(aNewPSO.RasterizerState.Get());
	myContext->OMSetDepthStencilState(aNewPSO.DepthStencilState.Get(), 0);

	for (const auto& [slot, sampler] : aNewPSO.SamplerStates)
	{
		myContext->VSSetSamplers(slot, 1, sampler.GetAddressOf());
		myContext->PSSetSamplers(slot, 1, sampler.GetAddressOf());
	}

	ComPtr<ID3D11VertexShader> vsShader;
	if (aNewPSO.VertexShader)
	{
		aNewPSO.VertexShader->myShader.As(&vsShader);
	}
	myContext->VSSetShader(vsShader.Get(), nullptr, 0);

	ComPtr<ID3D11GeometryShader> gsShader;
	if (aNewPSO.GeometryShader)
	{
		aNewPSO.GeometryShader->myShader.As(&gsShader);
	}
	myContext->GSSetShader(gsShader.Get(), nullptr, 0);

	ComPtr<ID3D11PixelShader> psShader;
	if (aNewPSO.PixelShader)
	{
		aNewPSO.PixelShader->myShader.As(&psShader);
	}
	myContext->PSSetShader(psShader.Get(), nullptr, 0);

	myContext->IASetInputLayout(aNewPSO.InputLayout.Get());


	if (aNewPSO.ClearRenderTarget)
	{
		myContext->ClearRenderTargetView(aNewPSO.RenderTarget->myRTV.Get(), aNewPSO.RenderTarget->myClearColor.data());
	}

	if (aNewPSO.ClearDepthStencil)
	{
		myContext->ClearDepthStencilView(aNewPSO.DepthStencil->myDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	if (aNewPSO.RenderTarget)
	{
		myContext->OMSetRenderTargets(1, aNewPSO.RenderTarget->myRTV.GetAddressOf(), aNewPSO.DepthStencil->myDSV.Get());
	}
	else
	{
		ID3D11RenderTargetView* rtv[] = { nullptr };
		myContext->OMSetRenderTargets(1, rtv, aNewPSO.DepthStencil->myDSV.Get());
	}

	D3D11_VIEWPORT viewport = {};
	if (!aNewPSO.RenderTarget && aNewPSO.DepthStencil)
	{
		viewport.TopLeftX = aNewPSO.DepthStencil->myViewport[0];
		viewport.TopLeftY = aNewPSO.DepthStencil->myViewport[1];
		viewport.Width = aNewPSO.DepthStencil->myViewport[2];
		viewport.Height = aNewPSO.DepthStencil->myViewport[3];
		viewport.MinDepth = aNewPSO.DepthStencil->myViewport[4];
		viewport.MaxDepth = aNewPSO.DepthStencil->myViewport[5];
	}
	else
	{
		viewport.TopLeftX	= myBackBuffer->myViewport[0];
		viewport.TopLeftY	= myBackBuffer->myViewport[1];
		viewport.Width		= myBackBuffer->myViewport[2];
		viewport.Height		= myBackBuffer->myViewport[3];
		viewport.MinDepth	= myBackBuffer->myViewport[4];
		viewport.MaxDepth	= myBackBuffer->myViewport[5];
	}

	myContext->RSSetViewports(1, &viewport);
}

void RenderHardwareInterface::CreateDefaultSamplerStates()
{
	D3D11_SAMPLER_DESC defaultSamplerDesc = {};
	defaultSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	defaultSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	defaultSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	defaultSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	defaultSamplerDesc.MipLODBias = 0.0f;
	defaultSamplerDesc.MaxAnisotropy = 1;
	defaultSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	defaultSamplerDesc.BorderColor[0] = 1.0f;
	defaultSamplerDesc.BorderColor[1] = 1.0f;
	defaultSamplerDesc.BorderColor[2] = 1.0f;
	defaultSamplerDesc.BorderColor[3] = 1.0f;
	defaultSamplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	defaultSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	CreateSamplerState("DefaultSS", defaultSamplerDesc);

	D3D11_SAMPLER_DESC shadowSamplerDesc = {};
	shadowSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.MipLODBias = 0.0f;
	shadowSamplerDesc.MaxAnisotropy = 1;
	shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	shadowSamplerDesc.BorderColor[0] = 1.0f;
	shadowSamplerDesc.BorderColor[1] = 1.0f;
	shadowSamplerDesc.BorderColor[2] = 1.0f;
	shadowSamplerDesc.BorderColor[3] = 1.0f;
	shadowSamplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	shadowSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	CreateSamplerState("ShadowSS", shadowSamplerDesc);

	D3D11_SAMPLER_DESC lutSamplerDesc = {};
	lutSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	lutSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	lutSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	lutSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	lutSamplerDesc.MipLODBias = 0.0f;
	lutSamplerDesc.MaxAnisotropy = 1;
	lutSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	lutSamplerDesc.BorderColor[0] = 0.0f;
	lutSamplerDesc.BorderColor[1] = 0.0f;
	lutSamplerDesc.BorderColor[2] = 0.0f;
	lutSamplerDesc.BorderColor[3] = 0.0f;
	lutSamplerDesc.MinLOD = 0;
	lutSamplerDesc.MaxLOD = 0;
	CreateSamplerState("LutSS", lutSamplerDesc);
}

bool RenderHardwareInterface::CreateSamplerState(std::string_view aName, const D3D11_SAMPLER_DESC& aSamplerDesc)
{
	ComPtr<ID3D11SamplerState> samplerState;
	const HRESULT result = myDevice->CreateSamplerState(&aSamplerDesc, &samplerState);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create sampler state {}", aName);
		return false;
	}

	SetObjectName(samplerState, aName);
	std::string mapKey(aName);
	mySamplerStates.emplace(mapKey, samplerState);

	LOG(RhiLog, Log, "Created sampler state {}", aName);
	return true;
}

const Microsoft::WRL::ComPtr<ID3D11SamplerState>& RenderHardwareInterface::GetSamplerState(const std::string& aName) const
{
	return mySamplerStates.at(aName);
}

bool RenderHardwareInterface::CreateRasterizerState(std::string_view aName, const D3D11_RASTERIZER_DESC& aRasterDesc, PipelineStateObject& aPSO)
{
	const HRESULT result = myDevice->CreateRasterizerState(&aRasterDesc, aPSO.RasterizerState.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create rasterizer state {}", aName);
		return false;
	}

	LOG(RhiLog, Log, "Created rasterizer state {}", aName);
	SetObjectName(aPSO.RasterizerState, aName);
	return true;
}

bool RenderHardwareInterface::LoadTexture(std::string_view aName, const uint8_t* aTextureDataPtr, size_t aTextureDataSize, Texture& outTexture) const
{
	ComPtr<ID3D11Resource> resource;
	const HRESULT result = DirectX::CreateDDSTextureFromMemory(myDevice.Get(), aTextureDataPtr, aTextureDataSize, &resource, &outTexture.mySRV);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create texture {} from memory", aName);
		return false;
	}
	
	D3D11_RESOURCE_DIMENSION resourceDimension = {};
	resource->GetType(&resourceDimension);
	switch (resourceDimension)
	{
		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		{
			ComPtr<ID3D11Texture2D> texture;
			resource.As(&texture);
			D3D11_TEXTURE2D_DESC desc = {};
			texture->GetDesc(&desc);
			outTexture.SetProperties(desc.ArraySize, desc.Usage, desc.BindFlags, desc.CPUAccessFlags);
			break;
		}
		default:
		{
			LOG(RhiLog, Error, "Engine does not support resources other than Texture2D at the moment!");
			return false;
			break;
		}
	}

	SetObjectName(resource, aName);
	std::string srvName(aName);
	srvName.append("_SRV");
	SetObjectName(outTexture.mySRV, srvName);
	LOG(RhiLog, Log, "Succesfully loaded texture {}", aName);

	return true;
}

bool RenderHardwareInterface::SetTextureResource(unsigned aPipelineStages, unsigned aSlot, const Texture& aTexture) const
{
	if (!(aTexture.myBindFlags & D3D11_BIND_SHADER_RESOURCE))
	{
		LOG(RhiLog, Error, "This texture can not be used as a resource!");
		return false;
	}

	if (aPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
	{
		myContext->VSSetShaderResources(aSlot, 1, aTexture.mySRV.GetAddressOf());
	}
	if (aPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
	{
		myContext->PSSetShaderResources(aSlot, 1, aTexture.mySRV.GetAddressOf());
	}

	return true;
}

bool RenderHardwareInterface::ClearTextureResourceSlot(unsigned aPipelineStages, unsigned aSlot) const
{
	ID3D11ShaderResourceView* dummySRV[] = { nullptr };
	if (aPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
	{
		myContext->VSSetShaderResources(aSlot, 1, dummySRV);
	}
	if (aPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
	{
		myContext->PSSetShaderResources(aSlot, 1, dummySRV);
	}

	return true;
}

bool RenderHardwareInterface::CreateShadowMap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture)
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = aWidth;
	desc.Height = aHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	HRESULT result;
	ComPtr<ID3D11Texture2D> texture;
	result = myDevice->CreateTexture2D(&desc, nullptr, reinterpret_cast<ID3D11Texture2D**>(texture.GetAddressOf()));
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create texture for shadow map {}", aName);
		return false;
	}
	std::string baseName(aName);
	SetObjectName(texture, baseName);


	D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc = {};
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	result = myDevice->CreateDepthStencilView(texture.Get(), &depthDesc, &outTexture.myDSV);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create depth stencil view for shadow map {}", aName);
		return false;
	}
	SetObjectName(outTexture.myDSV, baseName + "_DSV");


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;

	result = myDevice->CreateShaderResourceView(texture.Get(), &srvDesc, &outTexture.mySRV);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create shader resource view for shadow map {}", aName);
		return false;
	}
	SetObjectName(outTexture.mySRV, baseName + "_SRV");

	outTexture.SetProperties(desc.ArraySize, desc.Usage, desc.BindFlags, desc.CPUAccessFlags);
	outTexture.myViewport = { 0, 0, static_cast<float>(aWidth), static_cast<float>(aHeight), 0, 1 };

	return true;
}

bool RenderHardwareInterface::CreateShadowCubemap(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture)
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = aWidth;
	desc.Height = aHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 6;
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HRESULT result;
	ComPtr<ID3D11Texture2D> texture;
	result = myDevice->CreateTexture2D(&desc, nullptr, reinterpret_cast<ID3D11Texture2D**>(texture.GetAddressOf()));
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create texture for shadow cubemap {}", aName);
		return false;
	}
	std::string baseName(aName);
	SetObjectName(texture, baseName);


	D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc = {};
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	depthDesc.Texture2DArray.ArraySize = desc.ArraySize;
	depthDesc.Texture2DArray.FirstArraySlice = 0;
	depthDesc.Texture2DArray.MipSlice = 0;

	result = myDevice->CreateDepthStencilView(texture.Get(), &depthDesc, &outTexture.myDSV);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create depth stencil view for shadow cubemap {}", aName);
		return false;
	}
	SetObjectName(outTexture.myDSV, baseName + "_DSV");


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = desc.MipLevels;
	srvDesc.TextureCube.MostDetailedMip = 0;

	result = myDevice->CreateShaderResourceView(texture.Get(), &srvDesc, &outTexture.mySRV);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create shader resource view for shadow map {}", aName);
		return false;
	}
	SetObjectName(outTexture.mySRV, baseName + "_SRV");

	outTexture.SetProperties(desc.ArraySize, desc.Usage, desc.BindFlags, desc.CPUAccessFlags);
	outTexture.myViewport = { 0, 0, static_cast<float>(aWidth), static_cast<float>(aHeight), 0, 1 };

	return true;
}

bool RenderHardwareInterface::CreateLUT(std::string_view aName, unsigned aWidth, unsigned aHeight, Texture& outTexture)
{
	if (!CreateTexture(aName, aWidth, aHeight, RHITextureFormat::R16G16_Float, outTexture, false, true, true, false, false))
	{
		return false;
	}

#include "../Intermediate/Shaders/CompiledShaderHeaders/brdfLUT_VS.h"
#include "../Intermediate/Shaders/CompiledShaderHeaders/brdfLUT_PS.h"
	std::shared_ptr<Shader> LUTshaderVS = std::make_shared<Shader>();
	if (!LoadShaderFromMemory("LUT_VS", *LUTshaderVS, BuiltIn_brdfLUT_VS_ByteCode, sizeof(BuiltIn_brdfLUT_VS_ByteCode)))
	{
		LOG(RhiLog, Error, "Failed to load LUT vertex shader!");
		return false;
	}

	std::shared_ptr<Shader> LUTshaderPS = std::make_shared<Shader>();
	if (!LoadShaderFromMemory("LUT_PS", *LUTshaderPS, BuiltIn_brdfLUT_PS_ByteCode, sizeof(BuiltIn_brdfLUT_PS_ByteCode)))
	{
		LOG(RhiLog, Error, "Failed to load LUT pixel shader!");
		return false;
	}
	
	SetRenderTarget(outTexture);

	ComPtr<ID3D11VertexShader> vsShader;
	LUTshaderVS->myShader.As(&vsShader);
	myContext->VSSetShader(vsShader.Get(), nullptr, 0);
	
	ComPtr<ID3D11PixelShader> psShader;
	LUTshaderPS->myShader.As(&psShader);
	myContext->PSSetShader(psShader.Get(), nullptr, 0);

	SetPrimitiveTopology(Topology::TRIANGLESTRIP);
	SetVertexBuffer(nullptr, 0, 0);
	SetIndexBuffer(nullptr);
	SetInputLayout(nullptr);
	Draw(4);

	LOG(RhiLog, Log, "Successfully created LUT Texture {}!", aName);
	return true;
}

void RenderHardwareInterface::SetRenderTarget(Texture& aRenderTarget)
{
	myContext->OMSetRenderTargets(1, aRenderTarget.myRTV.GetAddressOf(), nullptr);
}

void RenderHardwareInterface::SetDepthStencil(Texture& aDepthStencil)
{
	ID3D11RenderTargetView* rtv[] = { nullptr };
	myContext->OMSetRenderTargets(1, rtv, aDepthStencil.myDSV.Get());
}

void RenderHardwareInterface::BeginEvent(std::string_view aEvent) const
{
	const std::wstring wideEvent = str::utf8_to_wide(aEvent.data());
	myUDA->BeginEvent(wideEvent.c_str());
}

void RenderHardwareInterface::EndEvent() const
{
	myUDA->EndEvent();
}

void RenderHardwareInterface::SetMarker(std::string_view aMarker) const
{
	const std::wstring wideMarker = str::utf8_to_wide(aMarker.data());
	myUDA->SetMarker(wideMarker.c_str());
}

bool RenderHardwareInterface::CreateVertexBufferInternal(std::string_view aName, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer,
														const uint8_t* aVertexDataPointer, size_t aNumVertices, size_t aVertexSize) const
{
	D3D11_BUFFER_DESC vxBufferDesc = {};
	vxBufferDesc.ByteWidth = static_cast<unsigned>(aNumVertices * aVertexSize);
	vxBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vxBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vxResource = {};
	vxResource.pSysMem = aVertexDataPointer;
	
	const HRESULT result = myDevice->CreateBuffer(&vxBufferDesc, &vxResource, outVxBuffer.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create Vertex Buffer!");
		return false;
	}

	SetObjectName(outVxBuffer, aName);

	return true;
}

bool RenderHardwareInterface::UpdateConstantBufferInternal(const ConstantBuffer& aBuffer, const void* aBufferData, size_t aBufferDataSize)
{
	if (!aBuffer.myBuffer)
	{
		LOG(RhiLog, Error, "Failed to update constant buffer. Buffer {} is invalid.", aBuffer.myName);
		return false;
	}

	if (aBufferDataSize > aBuffer.mySize)
	{
		LOG(RhiLog, Error, "Failed to update constant buffer. Data too large.");
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE bufferData = {};
	HRESULT result = myContext->Map(aBuffer.myBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to map constant buffer {}.", aBuffer.myName);
		return false;
	}

	memcpy_s(bufferData.pData, aBuffer.mySize, aBufferData, aBufferDataSize);
	myContext->Unmap(aBuffer.myBuffer.Get(), 0);

	return true;
}

void RenderHardwareInterface::SetObjectName(Microsoft::WRL::ComPtr<ID3D11DeviceChild> aObject, std::string_view aName) const
{
	if (aObject)
	{
		aObject->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<unsigned>(sizeof(char) * aName.size()), aName.data());
	}
}

bool RenderHardwareInterface::CreateTexture(std::string_view aName, unsigned aWidth, unsigned aHeight,
											RHITextureFormat aFormat, Texture& outTexture, bool aStaging, bool aShaderResource, bool aRenderTarget,
											bool aCpuAccessRead, bool aCpuAccessWrite) const
{
	// If this texture is a Staging texture or not.
	// Staging textures cannot be rendered to but are useful in i.e. screen picking.
	D3D11_USAGE usage = aStaging ? D3D11_USAGE_STAGING : D3D11_USAGE_DEFAULT;

	// Bind flags for this texture.
	// I.e. if it's used as a Resource (read)
	// or a Target (write)
	UINT bindFlags = {};
	if (aShaderResource)
	{
		bindFlags |= D3D11_BIND_SHADER_RESOURCE;
	}

	if (aRenderTarget)
	{
		bindFlags |= D3D11_BIND_RENDER_TARGET;
	}

	// CPU flags letting us read and/or write
	// from/to a texture. Imposes several restrictions.
	// Primary use in TGP is if you implement picking.
	UINT cpuFlags = {};
	if (aCpuAccessRead)
	{
		cpuFlags |= D3D11_CPU_ACCESS_READ;
	}
	if (aCpuAccessWrite)
	{
		cpuFlags |= D3D11_CPU_ACCESS_WRITE;
	}

	HRESULT result = E_FAIL;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = aWidth;
	desc.Height = aHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = static_cast<DXGI_FORMAT>(aFormat);
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = static_cast<D3D11_USAGE>(usage);
	desc.BindFlags = bindFlags;
	desc.CPUAccessFlags = cpuFlags;
	desc.MiscFlags = 0;

	outTexture.SetProperties(desc.ArraySize, desc.Usage, desc.BindFlags, desc.CPUAccessFlags);
	outTexture.myViewport = { 0, 0, static_cast<float>(aWidth), static_cast<float>(aHeight), 0, 1 };

	ComPtr<ID3D11Texture2D> texture;
	result = myDevice->CreateTexture2D(&desc, nullptr, reinterpret_cast<ID3D11Texture2D**>(texture.GetAddressOf()));
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create the texture {}!", aName);
		return false;
	}

	SetObjectName(texture, aName);

	if (aShaderResource)
	{
		result = myDevice->CreateShaderResourceView(texture.Get(), nullptr, &outTexture.mySRV);
		if (FAILED(result))
		{
			LOG(RhiLog, Error, "Failed to create the texture {}! Failed to create a shader resource view!", aName);
			return false;
		}

		SetObjectName(outTexture.mySRV, std::string(aName) + "_SRV");
	}

	if (aRenderTarget)
	{
		result = myDevice->CreateRenderTargetView(texture.Get(), nullptr, &outTexture.myRTV);
		if (FAILED(result))
		{
			LOG(RhiLog, Error, "Failed to create the texture {}! Failed to create a render target view!", aName);
			return false;
		}

		SetObjectName(outTexture.myRTV, std::string(aName) + "_RTV");
	}

	return true;
}
