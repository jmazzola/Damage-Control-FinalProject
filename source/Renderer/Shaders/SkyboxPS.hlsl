SamplerState TrilinearSampler : register(s0);

Texture2D DiffuseTexture : register(t0);

#include "VertexLayouts.hlsli"
#include "cbuffer.hlsli"

float4 main(Output_PositionTexture input) : SV_Target
{
	float3 color = DiffuseTexture.Sample(TrilinearSampler, input.texCoord + Translate.xy).rgb;

	return float4(color, 1);
}
