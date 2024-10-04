#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/ParticleIncludes.hlsli"
#include "../Includes/ConstantBuffers/FrameBuffer.hlsli"

[maxvertexcount(4)]
void main(
	point Particle_VSOut input[1], 
	inout TriangleStream<Particle_GSOut> output)
{    
    const float2 offsets[4] =
    {
        { -1.0f, 1.0f },
        { 1.0f, 1.0f },
        { -1.0f, -1.0f },
        { 1.0f, -1.0f }
    };
    const float2 uvs[4] =
    {
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f }
    };
    
    const ParticleVertex vertex = input[0];
    const float PI = 3.14159265358979323846f;
    const float DEGTORAD = PI / 180.0f;
    for (uint i = 0; i < 4; ++i)
    {
        Particle_GSOut result;
        result.Position = vertex.Position;
        float4 origin = result.Position;
        result.Position.xy += offsets[i] * vertex.Size;
        
        float radAngle = DEGTORAD * vertex.Angle;
        result.Position.xy = Rotate2DPointAroundPivot(origin.xy, result.Position.xy, radAngle);
        result.Position = mul(FB_Projection, result.Position);
        
        result.UV = uvs[i];
        
        result.Velocity = vertex.Velocity;
        result.GravityScale = vertex.GravityScale;
        result.Color = vertex.Color;
        result.Lifetime = vertex.Lifetime;
        result.ChannelMask = vertex.ChannelMask;
        output.Append(result);
    }
}