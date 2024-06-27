#pragma once
#include <filesystem>
#include <string>

#include "Asset.h"
#include "RHI/Mesh.h"
#include "RHI/Animation.h"
#include "RHI/Texture.h"
#include "Objects/Material.h"

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
