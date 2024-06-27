#include "../Includes/BRDF_Functions.hlsli"

float2 main(BRDF_VS_to_PS input) : SV_TARGET
{
    float2 integratedBRDF = IntegrateBRDF(input.UV.x, input.UV.y);
    return integratedBRDF;
}
