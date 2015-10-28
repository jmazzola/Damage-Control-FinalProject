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
};

//-------------------------------------------------------------------------------------------------
// Vertex shader entry point
//-------------------------------------------------------------------------------------------------
VSOutput main(in VSInput input)
{
	VSOutput output;

	// Calculate the clip-space position
	output.PositionCS = mul(float4(input.Position, 1.0f), WorldViewProjMatrix);

	// Pass along the texture coordinate
	output.TexCoord = input.TexCoord;

	return output;
}
