#include "../../Includes/DefaultShaderIncludes.hlsli"

DebugObject_VSout main(uint id : SV_VertexID)
{
    DebugObject_VSout result;
    result.vertexID = id;
	return result;
}