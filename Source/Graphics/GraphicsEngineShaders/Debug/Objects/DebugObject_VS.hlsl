#include "../../Includes/Debug_Includes.hlsli"

DebugObject_VSout main(uint id : SV_VertexID)
{
    DebugObject_VSout result;
    result.vertexID = id;
	return result;
}