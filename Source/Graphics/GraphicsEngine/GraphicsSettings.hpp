#pragma once
#include <unordered_map>
#include <cassert>
#include "CommonUtilities/StringUtilities.hpp"

namespace GraphicsSettings
{
	inline constexpr unsigned MAX_MATERIAL_TEXTURES = 8;
	inline constexpr unsigned MAX_PSO_TEXTURES = 8;
}

enum class ShaderType : unsigned
{
	Unknown,
	Vertex,
	Geometry,
	Pixel,
	COUNT
};

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

enum class BlendMode
{
	None,
	Alpha,
	Additive,
	COUNT
};

enum class FillMode
{
	Wireframe = 2,
	Solid = 3
};

enum class CullMode
{
	None = 1,
	Front = 2,
	Back = 3
};

enum class VertexType
{
	MeshVertex,
	DebugLineVertex,
	TextVertex,
	ParticleVertex,
	TrailVertex
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

	static const std::unordered_map<const char*, BlendMode> globalBlendModeNameToEnum
	{
		{	"none",				BlendMode::None			},
		{	"alpha",			BlendMode::Alpha,		},
		{	"additive",			BlendMode::Additive,	},
	};

	static const std::unordered_map<const char*, FillMode> globalFillModeNameToEnum
	{
		{	"wireframe",		FillMode::Wireframe,	},
		{	"solid",			FillMode::Solid			},
	};

	static const std::unordered_map<const char*, CullMode> globalCullModeNameToEnum
	{
		{	"none",				CullMode::None,		},
		{	"front",			CullMode::Front,	},
		{	"back",				CullMode::Back,		},
	};

	static const std::unordered_map<const char*, VertexType> globalVertexTypeNameToEnum
	{
		{	"mesh",					VertexType::MeshVertex,			},
		{	"line",					VertexType::DebugLineVertex,	},
		{	"text",					VertexType::TextVertex,			},
		{	"particle",				VertexType::ParticleVertex,		},
		{	"trail",				VertexType::TrailVertex,		},
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

inline BlendMode BlendModeFromName(const char* aBlendModeName)
{
	char blendModeName[512]{ 0 };
	Utilities::ToLowerCopy(aBlendModeName, blendModeName);
	assert(GraphicsSettings::globalBlendModeNameToEnum.contains(blendModeName));
	return GraphicsSettings::globalBlendModeNameToEnum.find(blendModeName)->second;
}

inline FillMode FillModeFromName(const char* aFillModeName)
{
	char fillModeName[512]{ 0 };
	Utilities::ToLowerCopy(aFillModeName, fillModeName);
	assert(GraphicsSettings::globalFillModeNameToEnum.contains(fillModeName));
	return GraphicsSettings::globalFillModeNameToEnum.find(fillModeName)->second;
}

inline CullMode CullModeFromName(const char* aCullModeName)
{
	char cullModeName[512]{ 0 };
	Utilities::ToLowerCopy(aCullModeName, cullModeName);
	assert(GraphicsSettings::globalCullModeNameToEnum.contains(cullModeName));
	return GraphicsSettings::globalCullModeNameToEnum.find(cullModeName)->second;
}

inline VertexType VertexTypeFromName(const char* aVertexTypeName)
{
	char vertexTypeName[512]{ 0 };
	Utilities::ToLowerCopy(aVertexTypeName, vertexTypeName);
	assert(GraphicsSettings::globalVertexTypeNameToEnum.contains(vertexTypeName));
	return GraphicsSettings::globalVertexTypeNameToEnum.find(vertexTypeName)->second;
}