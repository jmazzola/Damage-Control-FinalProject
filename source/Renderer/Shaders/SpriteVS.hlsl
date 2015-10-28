#pragma pack_matrix(row_major)

#include "VertexLayouts.hlsli"
#include "cbuffer.hlsli"

Output_PositionTexture main(Vertex_PositionTexture input)
{
	Output_PositionTexture output;

	output.position.xyz = input.position;
	output.position.w = 1.0f;

	output.texCoord = input.texCoord;

	return output;
}