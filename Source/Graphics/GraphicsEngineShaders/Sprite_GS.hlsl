#include "Includes/DefaultShaderIncludes.hlsli"
#include "Includes/Sprite_Includes.hlsli"
#include "Includes/ConstantBuffers/FrameBuffer.hlsli"

[maxvertexcount(4)]
void main(
	point Sprite_VSout input[1], 
	uint primID : SV_PrimitiveID,
	inout TriangleStream<Sprite_GSout> output)
{
    bool isScreenSpace = input[0].IsScreenSpace;
    float4x4 lsMatrix = input[0].LocalSpaceMatrix;
	
    float3 right = normalize(lsMatrix._11_12_13);
    float3 up = normalize(lsMatrix._21_22_23);
	
    float3 position = lsMatrix._41_42_43;
    float2 size = lsMatrix._11_22;
	
	if (isScreenSpace)
    {
        position.xy /= FB_Resolution;
        position.z = 0.0f;
        size /= FB_Resolution;
		
        right = -right;
    }
	
	float halfWidth = 0.5f * size.x;
    float halfHeight = 0.5f * size.y;
	
	float4 v[4];
	v[0] = float4(position + halfWidth * right - halfHeight * up, 1.0f);
	v[1] = float4(position + halfWidth * right + halfHeight * up, 1.0f);
	v[2] = float4(position - halfWidth * right - halfHeight * up, 1.0f);
	v[3] = float4(position - halfWidth * right + halfHeight * up, 1.0f);

	[unroll]
	for (uint i = 0; i < 4; i++)
	{
		Sprite_GSout element;
		
		if (isScreenSpace)
        {
			element.Position = v[i];
        }
		else
        {
			float4 viewPos = mul(FB_InvView, v[i]);
			float4 screenPos = mul(FB_Projection, viewPos);
			element.Position = screenPos;
        }
        
        element.WorldPos = v[i];
        element.TexCoord0 = defaultUVs[i];
        element.PrimID = primID;
		output.Append(element);
	}
}