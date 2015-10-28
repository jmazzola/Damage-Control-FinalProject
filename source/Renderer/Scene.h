/***********************************************
* Filename:  		Scene.h
* Date:      		05/12/2015
* Mod. Date: 		05/28/2015
* Mod. Initials:	DG
* Author:    		David Goad
* Purpose:   		Container class for objects
*						to eventually be drawn
************************************************/

#ifndef _CSCENE_H_
#define _CSCENE_H_

/*****************
* Necessary .h includes
******************/

#include "..\Core\stdafx.h"

#include "structs.h"

#include "ICamera.h"
#include "Skybox.h"

/*****************
* Class forward declarations
******************/
//class CICamera;
//class CSkybox;
class CRObject;
class CMagic;
class CEntity;

class CScene
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
	CScene( );

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
	~CScene( );

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
	void AddCamera(CICamera* pCamera);

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
	void AddSkybox(CSkybox* pSkybox);

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
	void AddObject(CRObject* pObject);

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
	void AddLight(const LightParams _pLight);

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
	void Update(DirectX::XMFLOAT3 bindPos, float dt);

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
	void ClearScene( )
	{
		for (int i = 0; i < CRObject::RObjectType::ROBJECT_COUNT; i++)
		{
			m_vRenderable[i].clear( );
		}

		m_mInstanced.clear( );
	}

	void ClearLights(bool _fPoint = false, bool _fSpot = false, bool _fDirectional = false)
	{
		if (_fPoint)
			m_vLights[LightType::Point].clear( );
		if (_fSpot)
			m_vLights[LightType::Spot].clear( );
		if (_fDirectional)
			m_vLights[LightType::Directional].clear( );

	}

	/***************
	*  Accessors
	***************/
	static CICamera* GetCamera( )
	{
		return m_pCamera;
	}
	static CSkybox* GetSkybox( )
	{
		return m_pSkybox;
	}

	//std::vector<CMagic*> GetLights( ) const
	//{
	//	return  m_vLights;
	//}
	std::vector<CRObject*> GetRenderable(CRObject::RObjectType _eRenderable) const
	{
		return m_vRenderable[_eRenderable];
	}

	std::vector<const LightParams> GetLightByType(int _nType)
	{
		return m_vLights[_nType];
	}

	/***************
	* Mutators
	***************/

	//i only need a single object
	//std::vector<CRObject*> obj;

	std::unordered_map<std::string, std::vector<CRObject*>> m_mInstanced;

private:
	bool CheckSphere(DirectX::XMFLOAT3 _XMPos, float radius);
	bool CheckRectangle(DirectX::XMFLOAT3 _XMPos, DirectX::XMFLOAT3 _XMSize);

	static CICamera* m_pCamera;
	static CSkybox* m_pSkybox;

	std::vector<const LightParams> m_vLights[3];

	std::vector<CRObject*> m_vRenderable[CRObject::RObjectType::ROBJECT_COUNT];
};

#endif // _CSCENE_H_
