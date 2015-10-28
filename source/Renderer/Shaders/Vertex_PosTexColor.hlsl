#include "VertexLayouts.hlsli"

float4 /*VertexPositionTextureColor*/main(Vertex_PositionTextureColor input) : SV_POSITION
{
	return float4(input.position, 1);
}
