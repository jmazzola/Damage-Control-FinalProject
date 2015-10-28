Texture2D texture2d : register(t0);

#include "../../Particle Manager/Particle.h"

SamplerState samplerstate : register(s0);

struct PS_OUT
{
	float4 color : SV_Target;
	float depth : SV_Depth;
};

float4 main(TParticleOutput input) : SV_Target
{
	return texture2d.Sample(samplerstate, input.uv) * input.color;
}