#pragma once

struct ID3D11BlendState;
struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11InputLayout;
struct ID3D11SamplerState;

class Shader;

struct PipelineStateObject
{
	PipelineStateObject();
	~PipelineStateObject();

	std::shared_ptr<PipelineStateObject> CreateInstance() const;

	Microsoft::WRL::ComPtr<ID3D11BlendState> BlendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout;
	std::unordered_map<unsigned, Microsoft::WRL::ComPtr<ID3D11SamplerState>> SamplerStates;

	unsigned VertexStride = 0;

	std::shared_ptr<Shader> VertexShader;
	std::shared_ptr<Shader> GeometryShader;
	std::shared_ptr<Shader> PixelShader;
};

