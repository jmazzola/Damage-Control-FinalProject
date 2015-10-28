/***********************************************
* Filename:  		RObject.h
* Date:				05/12/2015
* Mod. Date: 		05/28/2015
* Mod. Initials:	DG
* Author:    		David Goad
* Purpose:   		Base Class for all Renderable
*						objects
************************************************/

#ifndef _CROBJECT_H_
#define _CROBJECT_H_

#include "..\Core\stdafx.h"

#include "../Transform.h"

#include "../Math/Collision.h"

struct TRenderProperties;

class CRObject
{
public:
	enum RObjectType
	{
		ROBJECT_STATIC,
		ROBJECT_ANIMATED,
		ROBJECT_TRANSPARENT,
		ROBJECT_COUNT
	};

	/*****************************************************************
	* CRObject():
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		05/28/2015
	* Mod. Initials:	DG
	*****************************************************************/
	CRObject( );

	/*****************************************************************
	* CRObject():
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/12/2015
	* Mod. Initials:
	*****************************************************************/
	virtual ~CRObject( );

	/***************
	*  Accessors
	***************/
	virtual TCollider* GetCollider( )
	{
		return tCollider;
	}
	std::string GetTag( ) const
	{
		return m_szTag;
	}

	DirectX::XMFLOAT4 GetColor( ) const
	{
		return m_XMColor;
	}
	virtual DirectX::XMFLOAT4X4 GetModelWorld()
	{
		return MyWorld.GetWorld();
	}
	virtual DirectX::XMFLOAT4X4& GetWorldMatrix( )
	{
		return MyWorld.GetWorld();
	}
	virtual DirectX::XMFLOAT4X4& GetLocalMatrix( )
	{
		return MyWorld.Local;
	}
	/*virtual const DirectX::XMFLOAT4X4& GetWorldMatrix( ) const
	{
		return MyWorld.GetWorld();
	}*/
	DirectX::XMFLOAT3 GetXAxis( )
	{
		return DirectX::XMFLOAT3(MyWorld.GetWorld()._11, MyWorld.GetWorld()._12, MyWorld.GetWorld()._13);
	}
	DirectX::XMFLOAT3 GetYAxis( )
	{
		return DirectX::XMFLOAT3(MyWorld.GetWorld( )._21, MyWorld.GetWorld( )._22, MyWorld.GetWorld( )._23);
	}
	DirectX::XMFLOAT3 GetZAxis( )
	{
		return DirectX::XMFLOAT3(MyWorld.GetWorld( )._31, MyWorld.GetWorld( )._32, MyWorld.GetWorld( )._33);
	}
	DirectX::XMFLOAT3 GetPosition(void)
	{
		return DirectX::XMFLOAT3(MyWorld.GetWorld( )._41, MyWorld.GetWorld( )._42, MyWorld.GetWorld( )._43);
	}

	RObjectType GetRObjectType( ) const
	{
		return m_eRObjectType;
	}

	/***************
	* Mutators
	***************/
	void SetTag(std::string szTag)
	{
		m_szTag = szTag;
	}
	virtual void SetCollider(TCollider* _tCollider)
	{
		tCollider = _tCollider;
	}
	void SetColor(DirectX::XMFLOAT4 _XMColor)
	{
		m_XMColor = _XMColor;
	}
	void SetRObjectType(RObjectType eObjectType) { m_eRObjectType = eObjectType; }
	void SetLightParams(LightParams* pLight) { m_pLightParams = pLight; }

	//const TRenderProperties* RetrieveThyProperties( ) const
	//{
	//	return m_tpRenderProperties;
	//}

	virtual LightParams* GetLight( ) const
	{
		return m_pLightParams;
	}

	virtual bool HasLight( ) const
	{
		return m_pLightParams != NULL;
	}

	virtual void SetPosition(DirectX::XMFLOAT3& position);


protected:
	//DirectX::XMFLOAT4X4 m_XMWorldMatrix;
	std::string m_szTag;

	DirectX::XMFLOAT4 m_XMColor;
	DirectX::XMFLOAT4 m_XMBaseColor;
	TCollider* tCollider;

	//-lol- tp
	//TRenderProperties* m_tpRenderProperties;

	RObjectType m_eRObjectType;

	Transform MyWorld;

	LightParams* m_pLightParams;
};

#endif //_CROBJECT_H_
