#include "Includes/DefaultShaderIncludes.hlsli"

Sprite_VSout main()
{
    Sprite_VSout result;
    result.Position = SPB_Position;
    result.Size = SPB_Size;
	return result;
}