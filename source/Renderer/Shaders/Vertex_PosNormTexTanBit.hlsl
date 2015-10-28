#include "VertexLayouts.hlsli"

float4 /*VertexPositionNormalTextureTangentBiNormal*/main(Vertex_PositionNormalTextureTangentBiNormal input) : SV_POSITION
{
	return float4(input.position, 1);
}
