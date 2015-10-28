#pragma pack_matrix(row_major)

#include "VertexLayouts.hlsli"
#include "cbuffer.hlsli"

Output_PositionTextureColor main(Vertex_PositionTextureColor input)
{
	Output_PositionTextureColor output;

	output.position.xyz = input.position;// mul(input.position, gMVP);
	output.position.w = 1.0f;

	output.texCoord = input.texCoord;
	output.color = input.color;

	return output;
}
