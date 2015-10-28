/***********************************************
* Filename:  		PointLight.h
* Date:      		05/12/2015
* Mod. Date: 		05/28/2015
* Mod. Initials:	DG
* Author:    		David Goad
* Purpose:   		Point Lighting
************************************************/

#ifndef _CPOINTLIGHT_H_
#define _CPOINTLIGHT_H_

#include "..\Core\stdafx.h"

#include "Magic.h"

class CPointLight : public CMagic
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
	CPointLight( );

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
	~CPointLight( );

	/***************
	*  Accessors
	***************/

	/***************
	* Mutators
	***************/
};

#endif // _CPOINTLIGHT_H_
