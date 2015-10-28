/***********************************************
* Filename:  		ICamera.h
* Date:      		05/12/2015
* Mod. Date: 		05/28/2015
* Mod. Initials:	DG
* Author:    		David Goad
* Purpose:   		This serves as the base for
*						cameras. Can derive
*						for other behavior
************************************************/

#ifndef _ICAMERA_H_
#define _ICAMERA_H_

#include "../Core/stdafx.h"

#include "../Math/Collision.h"

#include <DirectXMath.h>

class CICamera
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
	CICamera( );

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
	virtual ~CICamera( );

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
	void UpdateView( );

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
	void UpdateProjection(float _fFieldOfView, float _fAspectRatio, float _fNearPlane, float _fFarPlane);

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
	virtual void Update(DirectX::XMFLOAT3 _XMBindPos, float _fDT) = 0;

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
	void ComputeFrustumPlanes(DirectX::XMFLOAT4X4& viewProj);

	DirectX::XMFLOAT4X4 View( );
	DirectX::XMFLOAT4X4 Projection( );
	DirectX::XMFLOAT4X4 ViewProjection( );

	DirectX::XMFLOAT4X4 InvView( );
	DirectX::XMFLOAT4X4 InvProjection( );

	DirectX::XMFLOAT4X4 Ortho( );

	DirectX::XMFLOAT3 Position( );

	std::vector<DirectX::XMFLOAT4>& GetFrustumPlanes( );

	DirectX::XMFLOAT2A ClipPlanes( )
	{
		return DirectX::XMFLOAT2A(m_fNear, m_fFar);
	}

protected:
	DirectX::XMFLOAT3 m_XMPosition;
	DirectX::XMFLOAT3 m_XMUp;
	DirectX::XMFLOAT3 m_XMLookAt;

	DirectX::XMFLOAT3 m_XMDirection;

	DirectX::XMFLOAT4X4 m_XMView;
	DirectX::XMFLOAT4X4 m_XMProjection;
	DirectX::XMFLOAT4X4 m_XMViewProjection;

	DirectX::XMFLOAT4X4 m_XMInvView;
	DirectX::XMFLOAT4X4 m_XMInvProjection;

	DirectX::XMFLOAT4X4 m_XMOrthoMatrix;

	float m_fFOV, m_fNear, m_fFar;
	float m_fWidth, m_fHeight;

private:

	std::vector<DirectX::XMFLOAT4> m_vPlanes;
};

#endif //_ICAMERA_H_
