#pragma once
#include <unordered_map>
#include <cassert>

namespace GraphicsSettings
{
	inline constexpr unsigned MAX_TEXTURES = 8;
}

enum class PSOType
{
	// OBJECTS
	Deferred,
	ForwardPBR,
	TransparentPBR,
	Shadow,
	ShadowCube,
	Sprite,
	Spritesheet,
	Text,
	Particle,
	Trail,
	ParticleSpritesheet,
	VFXSpritesheet,
	DebugLine,

	DeferredDirectionalLight,
	DeferredPointlight,
	DeferredSpotlight, 

	// POST PROCESSING
	Bloom,
	GaussianH,
	GaussianV,
	Luminance,
	RadialBlur,
	Resample,
	SSAO,
	TonemapACES,
	TonemapLottes,
	TonemapUE,

	// DEBUG
	Wireframe,
	DebugAO, // Combine into Gbuffer debug pso
	DebugRoughness, // Combine into Gbuffer debug pso
	DebugMetallic, // Combine into Gbuffer debug pso
	DebugPixelNormals, // Combine into Gbuffer debug pso
	DebugFX, // Combine into Gbuffer debug pso
	DebugUVs,
	DebugVertexColor,
	DebugVertexNormals,
	DebugTextureNormals
};

namespace GraphicsSettings
{
	static const std::unordered_map<PSOType, const char*> globalEnumToPSOName
	{
		{	PSOType::Deferred,					"Deferred"						},
		{	PSOType::ForwardPBR,				"Forward PBR"					},
		{	PSOType::TransparentPBR,			"Transparent PBR"				},
		{	PSOType::Shadow,					"Shadow"						},
		{	PSOType::ShadowCube,				"Shadow Cube"					},
		{	PSOType::Sprite,					"Sprite"						},
		{	PSOType::Spritesheet,				"Spritesheet"					},
		{	PSOType::Text,						"Text"							},
		{	PSOType::Particle,					"Particle"						},
		{	PSOType::Trail,						"Trail"							},
		{	PSOType::ParticleSpritesheet,		"Particle Spritesheet"			},
		{	PSOType::VFXSpritesheet,			"VFX Spritesheet"				},
		{	PSOType::DebugLine,					"Debug Line"					},
		{	PSOType::DeferredDirectionalLight,	"Deferred Directional Light"	},
		{	PSOType::DeferredPointlight,		"Deferred Pointlight"			},
		{	PSOType::DeferredSpotlight,			"Deferred Spotlight"			},
		{	PSOType::Bloom,						"Bloom"							},
		{	PSOType::GaussianH,					"GaussianH"						},
		{	PSOType::GaussianV,					"GaussianV"						},
		{	PSOType::Luminance,					"Luminance"						},
		{	PSOType::RadialBlur,				"Radial Blur"					},
		{	PSOType::Resample,					"Resample"						},
		{	PSOType::SSAO,						"SSAO"							},
		{	PSOType::TonemapACES,				"Tonemap ACES"					},
		{	PSOType::TonemapLottes,				"Tonemap Lottes"				},
		{	PSOType::TonemapUE,					"Tonemap UE"					},
		{	PSOType::Wireframe,					"Wireframe"						},
		{	PSOType::DebugAO,					"Debug AO"						},
		{	PSOType::DebugRoughness,			"Debug Roughness"				},
		{	PSOType::DebugMetallic,				"Debug Metallic"				},
		{	PSOType::DebugPixelNormals,			"Debug Pixel Normals"			},
		{	PSOType::DebugFX,					"Debug FX"						},
		{	PSOType::DebugUVs,					"Debug UVs"						},
		{	PSOType::DebugVertexColor,			"Debug Vertex Color"			},
		{	PSOType::DebugVertexNormals,		"Debug Vertex Normals"			},
		{	PSOType::DebugTextureNormals,		"Debug Texture Normals"			},
	};
}

inline const char* PSOName(PSOType aPSOType)
{
	assert(GraphicsSettings::globalEnumToPSOName.contains(aPSOType));
	return GraphicsSettings::globalEnumToPSOName.find(aPSOType)->second;
}