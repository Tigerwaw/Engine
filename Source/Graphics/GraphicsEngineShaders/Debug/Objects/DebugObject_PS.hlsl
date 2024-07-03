#include "../../Includes/DefaultShaderIncludes.hlsli"

float4 main(DebugObject_GSout input) : SV_TARGET
{
	return input.Color;
}