#include "VertexLayouts.hlsli"

float4 main(Vertex_PositionNormalTextureBoneWeights input) : SV_POSITION
{
	return float4(input.position, 1);
}
