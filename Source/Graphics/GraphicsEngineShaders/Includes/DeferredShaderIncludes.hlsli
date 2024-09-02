#ifndef _DEFERRED_INCLUDES_
#define _DEFERRED_INCLUDES_

Texture2D Def_Albedo : register(t0);
Texture2D Def_Material : register(t1);
Texture2D Def_WorldNormal : register(t2);
Texture2D Def_WorldPos : register(t3);

struct GBufferOutput
{
    float4 Albedo : SV_TARGET0;
    float4 Material : SV_TARGET1;
    float4 WorldNormal : SV_TARGET2;
    float4 WorldPosition : SV_TARGET3;
};

#endif // _DEFERRED_INCLUDES_