#pragma once
#include <filesystem>
#include <string>

#include "Asset.h"
#include "Graphics/GraphicsEngine/Objects/Mesh.h"
#include "Graphics/GraphicsEngine/Objects/Animation.h"
#include "Graphics/GraphicsEngine/Objects/Texture.h"
#include "Graphics/GraphicsEngine/Objects/Material.h"
#include "Graphics/GraphicsEngine/Objects/Shader.h"
#include "Graphics/GraphicsEngine/Objects/PipelineStateObject.h"

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
