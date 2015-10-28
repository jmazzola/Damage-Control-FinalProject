#pragma pack_matrix(row_major)

cbuffer Transforms : register(b0)
{
	matrix WorldMatrix;
	matrix WorldViewMatrix;
	matrix WorldViewProjMatrix;
};

struct VSInput
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 Normal	: NORMAL;
	float4 Tangent	: TANGENT;
};

struct VSOutput
{
	float4 PositionCS	: SV_Position;
	float2 TexCoord		: TEXCOORD;
	float3 NormalVS		: NORMALVS;
	float3 TangentVS	: TANGENTVS;
	float3 BitangentVS	: BITANGENTVS;
};

//-------------------------------------------------------------------------------------------------
// Vertex shader entry point
//-------------------------------------------------------------------------------------------------
VSOutput main(in VSInput input)
{
	VSOutput output;

	// Convert normals to view space
	float3 normalVS = normalize(mul(input.Normal, (float3x3)WorldViewMatrix));
	output.NormalVS = normalVS;

	// Reconstruct the rest of the tangent frame
	float3 tangentVS = normalize(mul(input.Tangent.xyz, (float3x3)WorldViewMatrix));
	float3 bitangentVS = normalize(cross(normalVS, tangentVS)) * input.Tangent.w;

	output.TangentVS = tangentVS;
	output.BitangentVS = bitangentVS;

	// Calculate the clip-space position
	output.PositionCS = mul(float4(input.Position, 1.0f), WorldViewProjMatrix);

	// Pass along the texture coordinate
	output.TexCoord = input.TexCoord;

	return output;
}