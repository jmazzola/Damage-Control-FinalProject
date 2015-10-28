#include "VertexLayouts.hlsli"

float4 /*VertexPositionTexture*/main(Vertex_PositionTexture input) : SV_POSITION
{
	return float4(input.position, 1);
}
