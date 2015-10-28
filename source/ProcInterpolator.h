/***********************************************
* Filename:			ProcInterpolator.h
* Date:      		05/12/2015
* Mod. Date:		05/19/2015
* Mod. Initials:	JPM
* Author:    		Justin Mazzola
* Purpose:			This class will be in charge
*					of interpolating keyframes or
*					channels' in a animation/pose.
************************************************/
#ifndef _CPROCINTERPOLATOR_H_
#define _CPROCINTERPOLATOR_H_

#include "Core\stdafx.h"
#include "Animation Manager\Interpolator.h"

class ProcInterpolator : CInterpolator <XMFLOAT4X4>
{
public:

	ProcInterpolator() {}

	~ProcInterpolator() {}

	__forceinline virtual void Process()
	{
		/*m_vPose.resize(2);

		m_fTotalAngle += m_fAngle;

		if (m_fTotalAngle >= 1.0f || m_fTotalAngle <= -1.0f)
			m_fAngle = -m_fAngle;

		m_vPose[0] = m_pAnim->GetChannels()[0].m_vKeys[0].m_Node;

		m_vPose[0].make_rotation_z(m_fTotalAngle);

		m_vPose[1] = m_vPose[0];
		m_vPose[1].translate_pre(4.0f, 0.0f, 0.0f);
		m_vPose[1].rotate_z_pre(-m_fTotalAngle*2.0f);*/
	}

private:

	float m_fTotalAngle;
	float m_fAngle;
};

#endif //_CPROCINTERPOLATOR_H_
