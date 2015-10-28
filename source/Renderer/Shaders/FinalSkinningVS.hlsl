#pragma pack_matrix(row_major)

const static int NUM_BONES = 100;

cbuffer Transforms : register(b0)
{
	matrix WorldMatrix;
	matrix WorldViewMatrix;
	matrix WorldViewProjMatrix;

	matrix SkinMatrices[NUM_BONES];

	float scaling;
};

struct VSInput
{
	//float3 Position	: POSITION;
	//float2 TexCoord	: TEXCOORD0;
	//float3 Normal		: NORMAL;
	//float4 Tangent	: TANGENT;

	float3 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL0;
	float4 Tangent : TANGENT0;
	float3 biNormal : BITANGENT;
	int cp : CONTROLPOINT0;
	int4 bone : BONE0;
	float4 weights : WEIGHTS0;
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
	float s = scaling;
	VSOutput output;

	//output.PositionCS = mul(float4(input.Position, 1.0f), WorldViewProjMatrix);

	// Calculate the clip-space position
	float3 positionCS = (float3)0;
	positionCS  = (mul(float4(input.Position.xyz, 1.0f), SkinMatrices[input.bone.x] * s) * input.weights.x).xyz;
	positionCS += (mul(float4(input.Position.xyz, 1.0f), SkinMatrices[input.bone.y] * s) * input.weights.y).xyz;
	positionCS += (mul(float4(input.Position.xyz, 1.0f), SkinMatrices[input.bone.z] * s) * input.weights.z).xyz;
	positionCS += (mul(float4(input.Position.xyz, 1.0f), SkinMatrices[input.bone.w] * s) * input.weights.w).xyz;

	output.PositionCS = mul(float4(positionCS, 1.0f), WorldViewProjMatrix);

	// Pass along the texture coordinate
	output.TexCoord = input.TexCoord;

	return output;
}
