#include "fRand.h"
#include "Math.h"
#include <cmath>
#include <Windows.h>
#include <DirectXMath.h>

#define PI 3.14159f

CFRand::CFRand(void)
{

}

float CFRand::next(float value)
{
	double seed = double(rand() % 10000) / 10000.0;

	return float(value * seed);
}

float CFRand::next(float min, float max)
{
	float range = abs(max - min);
	double seed = double(rand() % 10000) / 10000.0;

	return float(min + seed * (float)range);
}

v_type CFRand::nextVector(void)
{
	v_type outval;
	ZeroMemory(&outval, sizeof(v_type));

	float theta = next(2.f * PI);

	outval.z = next(-1.f, 1.f);
	outval.x = sqrt(1.f - outval.z*outval.z) * cos(theta);
	outval.y = sqrt(1.f - outval.z*outval.z) * sin(theta);
	
	DirectX::XMStoreFloat3(&outval, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&outval)));
	//outval = outval.normalize();
	return outval;
}