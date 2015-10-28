#include "VertexLayouts.hlsli"

float4 /*VertexPositionNormalTexture*/main(Vertex_PositionNormalTexture input) : SV_POSITION
{
	return float4(input.position, 1);
}
