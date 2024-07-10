#include "../../Includes/Debug_Includes.hlsli"

float4 main(DebugObject_GSout input) : SV_TARGET
{
	return input.Color;
}