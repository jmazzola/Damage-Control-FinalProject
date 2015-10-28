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
	float3 NormalVS		: NORMALVS;
	float3 TangentVS	: TANGENTVS;
	float3 BitangentVS	: BITANGENTVS;
};

//-------------------------------------------------------------------------------------------------
// Vertex shader entry point
//-------------------------------------------------------------------------------------------------
VSOutput main(in VSInput input)
{
	float s = scaling;
	VSOutput output;

	//float3 normalVS = normalize(mul(input.Normal, (float3x3)WorldViewMatrix));

	// Convert normals to view space
	float3 normalVS = (float3)0;
	normalVS  = (mul(float4(input.Normal, 1.0f), SkinMatrices[input.bone.x] * s) * input.weights.x).xyz;
	normalVS += (mul(float4(input.Normal, 1.0f), SkinMatrices[input.bone.y] * s) * input.weights.y).xyz;
	normalVS += (mul(float4(input.Normal, 1.0f), SkinMatrices[input.bone.z] * s) * input.weights.z).xyz;
	normalVS += (mul(float4(input.Normal, 1.0f), SkinMatrices[input.bone.w] * s) * input.weights.w).xyz;

	normalVS = normalize(mul(normalVS, (float3x3)WorldViewMatrix));
	output.NormalVS = normalVS;

	//float3 tangentVS = normalize(mul(input.Tangent.xyz, (float3x3)WorldViewMatrix));
	//float3 bitangentVS = normalize(cross(normalVS, tangentVS)) * input.Tangent.w;

	// Reconstruct the rest of the tangent frame
	float3 tangentVS = (float3)0;
	tangentVS  = (mul(float4(input.Tangent.xyz, 1.0f), SkinMatrices[input.bone.x] * s) * input.weights.x).xyz;
	tangentVS += (mul(float4(input.Tangent.xyz, 1.0f), SkinMatrices[input.bone.y] * s) * input.weights.y).xyz;
	tangentVS += (mul(float4(input.Tangent.xyz, 1.0f), SkinMatrices[input.bone.z] * s) * input.weights.z).xyz;
	tangentVS += (mul(float4(input.Tangent.xyz, 1.0f), SkinMatrices[input.bone.w] * s) * input.weights.w).xyz;

	tangentVS = normalize(mul(tangentVS, (float3x3)WorldViewMatrix));
	output.TangentVS = tangentVS;

	float3 bitangentVS = normalize(cross(normalVS, tangentVS)) * input.Tangent.w;
	output.BitangentVS = bitangentVS;

	//float3 positionCS = mul(float4(input.Position, 1.0f), WorldViewProjMatrix);

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