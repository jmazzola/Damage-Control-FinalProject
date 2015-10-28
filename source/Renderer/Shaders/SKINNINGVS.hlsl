#pragma pack_matrix(row_major)

#include "VertexLayouts.hlsli"
#include "cbuffer.hlsli"

//cbuffer TPerSkinnedObject : register(b1)
//{
//	const static int NUM_BONES = 200;
//	float4x4	gSkinnedWorld;
//
//	matrix	gSkinnedViewProj;
//
//	matrix	gSkinnedMatrices[NUM_BONES];
//};

Output_PositionTexture main(Vertex_PositionNormalTextureTangentBiNormalSkinned input)
{
	Output_PositionTexture output = (Output_PositionTexture)0;

	float3 Pos = (float3)0;
	{
		Pos  = (mul(float4(input.position, 1), perSkinned.gSkinMatrices[input.bone.x]) * input.weights.x).xyz;
		Pos += (mul(float4(input.position, 1), perSkinned.gSkinMatrices[input.bone.y]) * input.weights.y).xyz;
		Pos += (mul(float4(input.position, 1), perSkinned.gSkinMatrices[input.bone.z]) * input.weights.z).xyz;
		Pos += (mul(float4(input.position, 1), perSkinned.gSkinMatrices[input.bone.w]) * input.weights.w).xyz;
	}

	output.position = mul(float4(Pos, 1.0f), perSkinned.gSkinnedMVP);

	output.texCoord = input.texCoord;

	return output;
}
