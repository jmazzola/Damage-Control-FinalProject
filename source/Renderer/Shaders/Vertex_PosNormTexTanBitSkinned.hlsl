#include "VertexLayouts.hlsli"

float4 /*VertexPositionNormalTextureTangentBiNormalSkinned*/main(Vertex_PositionNormalTextureTangentBiNormalSkinned input) : SV_POSITION
{
	return float4(input.position, 1);
}
