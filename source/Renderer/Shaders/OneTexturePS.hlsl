Texture2D texture2d : register(t0);

SamplerState samplerstate : register(s0);

#include "VertexLayouts.hlsli"
#include "cbuffer.hlsli"

float4 main(Output_PositionTexture input) : SV_TARGET
{
	return texture2d.Sample(samplerstate, input.texCoord) * colorMod;
}
