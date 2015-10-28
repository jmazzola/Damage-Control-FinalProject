#include "VertexLayouts.hlsli"

float4 /*VertexPosition*/main(Vertex_Position input) :SV_POSITION
{
	return float4(input.position, 1);
}
