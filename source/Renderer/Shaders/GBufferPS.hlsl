#pragma pack_matrix(row_major)

struct PSInput
{
	float4 PositionSS 			: SV_Position;
	float2 TexCoord				: TEXCOORD;
	float3 NormalVS				: NORMALVS;
	float3 TangentVS			: TANGENTVS;
	float3 BitangentVS			: BITANGENTVS;
};

//-------------------------------------------------------------------------------------------------
// Textures
//-------------------------------------------------------------------------------------------------
Texture2D		NormalMap : register(t0);
SamplerState    AnisoSampler : register(s0);

//-------------------------------------------------------------------------------------------------
// Function for encoding normals using a spheremap transform
//-------------------------------------------------------------------------------------------------
float2 SpheremapEncode(float3 normalVS)
{
	return normalize(normalVS.xy) * (sqrt(-normalVS.z * 0.5f + 0.5f));
}

//-------------------------------------------------------------------------------------------------
// Pixel shader entry point
//-------------------------------------------------------------------------------------------------
float4 main(in PSInput input, in uint coverageMask : SV_Coverage) : SV_Target0
{
	//return float4(normalize(input.NormalVS), 1.0f);
	//return float4(normalize(input.TangentVS), 1.0f);
	//return float4(normalize(input.BitangentVS), 1.0f);

	// Normalize the tangent frame after interpolation
	float3x3 tangentFrameVS = float3x3(normalize(input.TangentVS),
									   normalize(input.BitangentVS),
									   normalize(input.NormalVS));

	// Sample the tangent-space normal map and decompress
	float3 normalTS = normalize(NormalMap.Sample(AnisoSampler, input.TexCoord).rgb * 2.0f - 1.0f);

	// Convert to view space
	float3 normalVS = mul(normalTS, tangentFrameVS);

	// The coverage mask tells which which MSAA sample points passed the triangle coverage
	// test. We can use this to determine if we're currently shading an edge pixel, which
	// is a pixel where the triangle doesn't fully cover all of the sample points
	const uint NumMSAASamples = 4;
	const uint FullMask = (1 << NumMSAASamples) - 1; // 0xF
	float edgePixel = coverageMask != FullMask ? 1.0f : 0.0f;

	// Output our G-Buffer values
	float2 normalEncoded = SpheremapEncode(normalVS);
	return float4(normalEncoded, pow(2, 5), edgePixel);
}