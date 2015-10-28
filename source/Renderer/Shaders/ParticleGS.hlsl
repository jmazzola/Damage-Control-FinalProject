#pragma pack_matrix(row_major)
#include "cbuffer.hlsli"
#include "../../Particle Manager/Particle.h"

StructuredBuffer<CParticle> particles : register(t0);




[maxvertexcount(6)]
void main(point int vertex[1] : INDEX, inout TriangleStream<TParticleOutput> output)
{
	float4 QuadPoints[6] =
	{
		{ -10.f, -10.f, 0.f, 1.f },
		{ 10.f, -10.f, 0.f, 1.f },
		{ -10.f, 10.f, 0.f, 1.f },

		{ -10.f, 10.f, 0.f, 1.f },
		{ 10.f, -10.f, 0.f, 1.f },
		{ 10.f, 10.f, 0.f, 1.f },
	};

	float2 texCoords[6] =
	{
		{ 0.f, 0.f },
		{ 1.f, 0.f },
		{ 0.f, 1.f },
		{ 0.f, 1.f },
		{ 1.f, 0.f },
		{ 1.f, 1.f },
	};

	CParticle p = particles[vertex[0]];
	if (p.m_fLifeTime > 0.f)
	{
		for (int i = 0; i < 6; i++)
		{
			TParticleOutput outval = (TParticleOutput)0;
			float4 pos = float4(p.m_vPosition, 1.f);
			float angle = atan2(QuadPoints[i].y, QuadPoints[i].x);
			angle += p.m_fRotation;
			float4 QuadPoint = float4(cos(angle), sin(angle), 0.f, 0.f);

			pos = mul(pos, mWorld);
			pos = mul(pos, mView);
			pos += QuadPoint * p.m_fSize;
			pos = mul(pos, mProj);

			outval.position = pos;
			outval.color = p.m_blendColor; 
			outval.uv = texCoords[i];

			output.Append(outval);
		}

		//TParticleOutput outval = (TParticleOutput)0;
		//float4 pos = mul(float4(p.m_vPosition, 1.f), mWorld);
		//pos = mul(pos, mView);
		//pos = mul(pos, mProj);
		//outval.position = pos;
		//outval.color = p.m_blendColor;

		//output.Append(outval);
	}
}