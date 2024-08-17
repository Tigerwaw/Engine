#pragma once
#include "Objects/Mesh.h"
#include "Objects/Animation.h"
#include "Objects/Texture.h"
#include "Objects/Material.h"
#include "Objects/Shader.h"
#include "Objects/PipelineStateObject.h"

struct Asset
{
	virtual ~Asset() = default;
	std::filesystem::path path;
	std::filesystem::path name;
};

struct MeshAsset : public Asset
{
	std::shared_ptr<Mesh> mesh;
};

struct AnimationAsset : public Asset
{
	std::shared_ptr<Animation> animation;
};

struct TextureAsset : public Asset
{
	std::shared_ptr<Texture> texture;
};

struct MaterialAsset : public Asset
{
	std::shared_ptr<Material> material;
};

struct ShaderAsset : public Asset
{
	std::shared_ptr<Shader> shader;
};

struct PSOAsset : public Asset
{
	std::shared_ptr<PipelineStateObject> pso;
};
