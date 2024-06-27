#pragma once
#include <memory>
#include <string>

class Texture;

struct VertexElementDesc
{
	std::string Semantic;
	unsigned SemanticIndex = 0;
	unsigned Type = 0;
};

typedef enum PIPELINE_STAGE
{
	PIPELINE_STAGE_INPUT_ASSEMBLER = 0x1L,
	PIPELINE_STAGE_VERTEX_SHADER = 0x2L,
	PIPELINE_STAGE_GEOMETRY_SHADER = 0x4L,
	PIPELINE_STAGE_RASTERIZER = 0x8L,
	PIPELINE_STAGE_PIXEL_SHADER = 0x10L,
	PIPELINE_STAGE_OUTPUT_MERGER = 0x20L,
} PIPELINE_STAGE;

enum class RHITextureFormat : unsigned
{
	R32G32B32A32_Float = 2,
	R32G8X24_Typeless = 19,
	D32_Float_S8X24_UINT = 20,
	R32_Float_X8X24_Typeless = 21,
	R16G16_Float = 34,
	R32_Typeless = 39,
	D32_Float = 40,
	R32_Float = 41,
};

enum class Topology : unsigned
{
	LINELIST = 2,
	TRIANGLELIST = 4,
	TRIANGLESTRIP = 5
};