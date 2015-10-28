#ifndef _FRAND_H
#define _FRAND_H
#include "vec3.h"

typedef DirectX::XMFLOAT3 v_type;

class CFRand
{
public:
	CFRand(void);

	float next(float min, float max);
	float next(float value);

	v_type nextVector(void);
};

#endif