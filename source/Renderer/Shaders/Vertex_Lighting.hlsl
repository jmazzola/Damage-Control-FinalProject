#include "VertexLayouts.hlsli"

float4 main(Vertex_Light input) : SV_POSITION
{
	return float4(input.Position, 1.0f);
}