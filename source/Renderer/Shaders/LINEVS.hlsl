#pragma pack_matrix(row_major)

#include "VertexLayouts.hlsli"
#include "cbuffer.hlsli"

float4 main(Vertex_Position input) : SV_POSITION
{
	float4 pos = mul(float4(input.position, 1), gView);
	pos = mul(pos, gProj);
	return pos;
}
