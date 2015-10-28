#pragma pack_matrix(row_major)

#include "VertexLayouts.hlsli"
#include "cbuffer.hlsli"

Output_PositionTexture main(Vertex_PositionTexture input)
{
	Output_PositionTexture output = (Output_PositionTexture)0;

	//output.position = mul(mul(float4(input.position, 1.0f), gView), gProj);
	//output.position = mul(float4(input.position, 1.0f), gMVP);
	output.position = float4(input.position, 1.0f);
	output.texCoord = input.texCoord;// +Translate;

	return output;
}
