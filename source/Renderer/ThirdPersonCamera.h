/***********************************************
* Filename:  		ThirdPersonCamera.h
* Date:				05/12/2015
* Mod. Date: 		05/28/2015
* Mod. Initials:	DG
* Author:    		David Goad
* Purpose:   		Chase Camera class that follows
*						the player
************************************************/

#ifndef _CHASE_CAMERA_H
#define _CHASE_CAMERA_H

#include "../Core/stdafx.h"

//leftright is the offset in the x-axis
//updown is the offset in the y-axis
//zindex is the offset in the z-axis
#define LEFTRIGHT	 0.0f
#define UPDOWN     700.0f//450.0f
#define ZINDEX    -400.0f

#include "ICamera.h"
class CThirdPersonCamera : public CICamera
{
public:
	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	CThirdPersonCamera( );

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void Update(DirectX::XMFLOAT3 _XM_XMBindPos,
				float _fDT) override;

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void Reset( );

	DirectX::XMFLOAT3 GetPositionOffset() { return m_XMDesiredPositionOffset; }

private:

	float m_fStiffness;
	float m_fDamping;
	float m_fMass;

	DirectX::XMFLOAT3 m_XMVelocity;
	DirectX::XMFLOAT3 m_XMDesiredPosition;
	DirectX::XMFLOAT3 m_XMDesiredPositionOffset;
	DirectX::XMFLOAT3 m_XMLookAtOffset;
};

#endif //_CHASE_CAMERA_H
