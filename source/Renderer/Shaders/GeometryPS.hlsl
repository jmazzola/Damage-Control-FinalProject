#include "VertexLayouts.hlsli"
#include "cbuffer.hlsli"

Texture2D		texture2d : register(t0);
SamplerState    samplerstate : register(s0);

float2 SpheremapEncode(float3 normalVS)
{
	return normalize(normalVS.xy) * (sqrt(-normalVS.z * 0.5f + 0.5f));
}

float4 main(in Output_PositionNormalTextureTangentBiNormal input, in uint coverageMask : SV_Coverage) : SV_Target0
{
	// Normalize the tangent frame after interpolation
	float3x3 tangentFrameVS = float3x3(normalize(input.tangent),
	normalize(input.biNormal),
	normalize(input.normal));

	// Sample the tangent-space normal map and decompress
	float3 normalTS = normalize(texture2d.Sample(samplerstate, input.texCoord).rgb * 2.0f - 1.0f);

	// Convert to view space
	float3 normalVS = mul(normalTS, tangentFrameVS);

	//[NOTE: THIS IS FOR LATERS]
	// The coverage mask tells which which MSAA sample points passed the triangle coverage
	// test. We can use this to determine if we're currently shading an edge pixel, which
	// is a pixel where the triangle doesn't fully cover all of the sample points
	const uint NumMSAASamples = 4;
	const uint FullMask = (1 << NumMSAASamples) - 1; // 0xF
	float edgePixel = coverageMask != FullMask ? 1.0f : 0.0f;
	//[END NOTE]

	// Output our G-Buffer values
	float2 normalEncoded = SpheremapEncode(normalVS);
	return float4(normalEncoded, 64.0f, edgePixel);
}