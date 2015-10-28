/***********************************************
* Filename:  		HUDRenderer.h
* Date:      		05/12/2015
* Mod. Date: 		05/28/2015
* Mod. Initials:	DG
* Author:    		David Goad
* Purpose:   		This class is responsible
*						for drawing 2D
************************************************/

#ifndef _HUDRENDERER_H_
#define _HUDRENDERER_H_

#include "../Core/stdafx.h"
#include "ConstantBufferStructures.h"

//Canibalistic
//Humanoid
//Underground
//Dweller

class CTexture2D;

class CHUDRenderer
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
	CHUDRenderer( );

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
	~CHUDRenderer(void);

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
	static void Initialize( );

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
	static void Shutdown( );

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
	static void Render2D(CTexture2D* _pTexture2D);

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
	static void SetTexCoordData(DirectX::XMFLOAT4 _pTexCoords[4]);

private:

	static ID3D11Buffer*	m_d3dIndexBuffer;
	static ID3D11Buffer*	m_d3dVertexBuffer;

	static ID3D11Buffer*	m_d3dTexCoordCBuffer;
};

#endif //_HUDRENDERER_H_
