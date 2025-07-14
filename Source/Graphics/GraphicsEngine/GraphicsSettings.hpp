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
	DebugTextureNormals,
	COUNT
};

enum class SamplerType
{
	PointWrap,
	LinearWrap,
	AnisotropicWrap,
	PointClamp,
	LinearClamp,
	AnisotropicClamp,
	Shadow,
	LUT,
	COUNT
};

enum class ConstantBufferType
{
	ObjectBuffer,
	FrameBuffer,
	AnimationBuffer,
	MaterialBuffer,
	LightBuffer,
	ShadowBuffer,
	SpriteBuffer,
	PostProcessBuffer,
	COUNT
};

enum class TonemapperType
{
	UE,
	ACES,
	Lottes,
	COUNT
};

enum class LuminanceType
{
	RandomGain,
	Contrast,
	ReductionAndGain,
	Fade,
	COUNT
};

enum class BloomType
{
	Additive,
	ScaledToScene,
	ScaledToLuminance,
	COUNT
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

	static const std::unordered_map<SamplerType, const char*> globalEnumToSamplerName
	{
		{	SamplerType::PointWrap,					"PointWrap"					},
		{	SamplerType::LinearWrap,				"LinearWrap"				},
		{	SamplerType::AnisotropicWrap,			"AnisotropicWrap"			},
		{	SamplerType::PointClamp,				"PointClamp"				},
		{	SamplerType::LinearClamp,				"LinearClamp"				},
		{	SamplerType::AnisotropicClamp,			"AnisotropicClamp"			},
		{	SamplerType::Shadow,					"Shadow"					},
		{	SamplerType::LUT,						"LUT"						},
	};

	static const std::unordered_map<ConstantBufferType, const char*> globalEnumToConstantBufferName
	{
		{	ConstantBufferType::ObjectBuffer,			"ObjectBuffer"				},
		{	ConstantBufferType::FrameBuffer,			"FrameBuffer"				},
		{	ConstantBufferType::AnimationBuffer,		"AnimationBuffer"			},
		{	ConstantBufferType::MaterialBuffer,			"MaterialBuffer"			},
		{	ConstantBufferType::LightBuffer,			"LightBuffer"				},
		{	ConstantBufferType::ShadowBuffer,			"ShadowBuffer"				},
		{	ConstantBufferType::SpriteBuffer,			"SpriteBuffer"				},
		{	ConstantBufferType::PostProcessBuffer,		"PostProcessBuffer"			},
	};

	static const std::unordered_map<TonemapperType, const char*> globalEnumToTonemapperName
	{
		{	TonemapperType::ACES,			"ACES"			},
		{	TonemapperType::Lottes,			"Lottes"		},
		{	TonemapperType::UE,				"UE"			},
	};

	static const std::unordered_map<LuminanceType, const char*> globalEnumToLuminanceName
	{
		{	LuminanceType::RandomGain,			"Random Gain"			},
		{	LuminanceType::Contrast,			"Contrast"				},
		{	LuminanceType::ReductionAndGain,	"Reduction and Gain"	},
		{	LuminanceType::Fade,				"Fade"					},
	};

	static const std::unordered_map<BloomType, const char*> globalEnumToBloomName
	{
		{	BloomType::Additive,				"Additive"				},
		{	BloomType::ScaledToScene,			"Scaled to Scene"		},
		{	BloomType::ScaledToLuminance,		"Scaled to Luminance"	},
	};
}

inline const char* PSOName(PSOType aPSOType)
{
	assert(GraphicsSettings::globalEnumToPSOName.contains(aPSOType));
	return GraphicsSettings::globalEnumToPSOName.find(aPSOType)->second;
}

inline const char* SamplerName(SamplerType aSamplerType)
{
	assert(GraphicsSettings::globalEnumToSamplerName.contains(aSamplerType));
	return GraphicsSettings::globalEnumToSamplerName.find(aSamplerType)->second;
}

inline const char* ConstantBufferName(ConstantBufferType aConstantBufferType)
{
	assert(GraphicsSettings::globalEnumToConstantBufferName.contains(aConstantBufferType));
	return GraphicsSettings::globalEnumToConstantBufferName.find(aConstantBufferType)->second;
}

inline const char* TonemapperName(TonemapperType aTonemapperType)
{
	assert(GraphicsSettings::globalEnumToTonemapperName.contains(aTonemapperType));
	return GraphicsSettings::globalEnumToTonemapperName.find(aTonemapperType)->second;
}

inline const char* LuminanceName(LuminanceType aLuminanceType)
{
	assert(GraphicsSettings::globalEnumToLuminanceName.contains(aLuminanceType));
	return GraphicsSettings::globalEnumToLuminanceName.find(aLuminanceType)->second;
}

inline const char* BloomName(BloomType aBloomType)
{
	assert(GraphicsSettings::globalEnumToBloomName.contains(aBloomType));
	return GraphicsSettings::globalEnumToBloomName.find(aBloomType)->second;
}