#include "cbuffer.hlsli"

//cbuffer TColor : register(b0)
//{
//	float4 colorMod;
//}

float4 main( ) : SV_TARGET
{
	return colorMod;// float4(2.0f, 2.0f, 2.0f, 1.0f);
}
