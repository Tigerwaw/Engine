#include "Includes/DefaultShaderIncludes.hlsli"

[maxvertexcount(4)]
void main(
	point Sprite_VSout input[1], 
	uint primID : SV_PrimitiveID,
	inout TriangleStream<Sprite_GSout> output)
{
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 right = float3(-1.0f, 0.0f, 0.0f);
	
	float halfWidth = 0.5f * input[0].Size.x;
    float halfHeight = 0.5f * input[0].Size.y;
	
	float4 v[4];
	v[0] = float4(input[0].Position.xyz + halfWidth * right - halfHeight * up, 1.0f);
	v[1] = float4(input[0].Position.xyz + halfWidth * right + halfHeight * up, 1.0f);
	v[2] = float4(input[0].Position.xyz - halfWidth * right - halfHeight * up, 1.0f);
	v[3] = float4(input[0].Position.xyz - halfWidth * right + halfHeight * up, 1.0f);

	[unroll]
	for (uint i = 0; i < 4; i++)
	{
		Sprite_GSout element;
        //float4 viewPos = mul(FB_InvView, v[i]);
		//float4 screenPos = mul(FB_Projection, viewPos);
        //element.Position = screenPos;
        element.Position = v[i];
        element.Position.x /= FB_Resolution.x;
        element.Position.y /= FB_Resolution.y;
        element.WorldPos = v[i];
        element.TexCoord0 = defaultUVs[i];
        element.PrimID = primID;
		output.Append(element);
	}
}