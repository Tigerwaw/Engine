#include "../../Includes/Debug_Includes.hlsli"

[maxvertexcount(2)]
void main(point DebugLine_VSout input[1],
		  inout LineStream<DebugObject_GSout> output)
{
	DebugObject_GSout fromElement;
    fromElement.Position = input[0].From;
    fromElement.Color = input[0].Color;
	output.Append(fromElement);
	
	DebugObject_GSout toElement;
    toElement.Position = input[0].To;
    toElement.Color = input[0].Color;
	output.Append(toElement);
}