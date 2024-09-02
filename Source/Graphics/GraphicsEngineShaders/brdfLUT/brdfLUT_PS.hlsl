#include "../Includes/BRDF_Functions.hlsli"
#include "../Includes/DefaultShaderIncludes.hlsli"

float2 main(Quad_VS_to_PS input) : SV_TARGET
{
    float2 integratedBRDF = IntegrateBRDF(input.UV.x, input.UV.y);
    return integratedBRDF;
}
