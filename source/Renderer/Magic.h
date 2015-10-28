/***********************************************
* Filename:  		Magic.h
* Date:      		05/12/2015
* Mod. Date: 		05/28/2015
* Mod. Initials:	DG
* Author:    		David Goad
* Purpose:   		Base class for lighting
************************************************/

#ifndef _CMAGIC_H_
#define _CMAGIC_H_

#include "..\Core\stdafx.h"

class CMagic
{
public:
	enum eLightType
	{
		ePoint, eSpot
	};

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
	CMagic( );

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
	~CMagic( );

	/***************
	*  Accessors
	***************/
	eLightType GetLightType( ) const
	{
		return m_eLightType;
	}

	/***************
	* Mutators
	***************/

protected:
	eLightType m_eLightType;

};

struct DirectionalLight
{
	__forceinline DirectionalLight( );

	DirectX::XMFLOAT4A mAmbientColor;
	DirectX::XMFLOAT4A mDiffuseColor;
	DirectX::XMFLOAT4A mSpecularColor;
	DirectX::XMFLOAT3A mDirection;
};

DirectionalLight::DirectionalLight( )
{
	memset(this, 0, sizeof(DirectionalLight));
}

struct PointLight
{
	__forceinline PointLight( );

	DirectX::XMFLOAT4A mAmbientColor;
	DirectX::XMFLOAT4A mDiffuseColor;
	DirectX::XMFLOAT4A mSpecularColor;

	DirectX::XMFLOAT3A mAttenuation;

	DirectX::XMFLOAT3 mPosition;
	float mRange;
};

PointLight::PointLight( )
{
	memset(this, 0, sizeof(PointLight));
}

struct SpotLight
{
	__forceinline SpotLight( );

	DirectX::XMFLOAT4A mAmbientColor;
	DirectX::XMFLOAT4A mDiffuseColor;
	DirectX::XMFLOAT4A mSpecularColor;

	DirectX::XMFLOAT3A mDirection;

	DirectX::XMFLOAT3 mPosition;
	float mRange;

	DirectX::XMFLOAT3 mAttenuation;
	float mSpotFactor;
};

SpotLight::SpotLight( )
{
	memset(this, 0, sizeof(SpotLight));
}

#endif //_CMAGIC_H_
