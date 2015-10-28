/***********************************************
* Filename:			Entity.h
* Date:      		05/12/2015
* Mod. Date:		05/14/2015
* Mod. Initials:	BL
* Author:    		Jagoba Marcos
* Purpose:			Base class for all the entities,
*					some will only be an entity.
************************************************/

#ifndef _CENTITY_H_
#define _CENTITY_H_

#include "..\Core\stdafx.h"
#include "../Renderer/RObject.h"
#include "..\Core\EnumerationHolder.h"

class CEntity : public CRObject
{
public:
	enum eEntityType
	{
		ePLAYER, ePICKUP, eBULLET, eCREEP, eHUNTER, /*eSPAMMER,*/ eSABOTEUR,
		eSUBSYSTEM, eWORKSTATION = 8, eDOOR = 11, eMAP, eENEMYBULLET,
		/*eLIFESUPPORT, eENGINES, eREACTOR,*/
		eCOUNT
	};

	CEntity( );
	virtual ~CEntity( );

	virtual void Update(float dt);

	// Enable: Turns on an enemy, and sets their spawn point
	/*****************************************************************
	* Enable():					Sets an entitys values back to their defaults
	*							and spawns it.
	* Ins:						DirectX::XMFLOAT3
	* Outs:						void
	* Returns:					void
	* Mod. Date:				05/18/2015
	* Mod. Initials:			BL
	*****************************************************************/
	virtual void Enable(DirectX::XMFLOAT3 SpawnPoint, eEnemyVariant variant = eEnemyVariant::eNoVariant);

	// Disable: Turns an enemy off, cleans up their stuff
	/*****************************************************************
	* Disable():				Turns an entity off, cleans up any necessary memory
	* Ins:						DirectX::XMFLOAT3
	* Outs:						void
	* Returns:					void
	* Mod. Date:				05/18/2015
	* Mod. Initials:			BL
	*****************************************************************/
	virtual void Disable( );

	/***************
	*  Accessors
	***************/
	//DirectX::XMFLOAT4X4& GetWorldMatrix() { return m_XMWorldMatrix; }
	//std::string GetTag() { return m_szTag; }
	virtual unsigned int* GetBucketNumber( )
	{
		return unBucketNumber;
	}
	DirectX::XMFLOAT3 GetPosition( );
	virtual eEntityType GetEntityType( ) const
	{
		return m_nEntityType;
	}

	/***************
	* Mutators
	***************/
	virtual void SetBucketNumber(unsigned int unBucket, int index)
	{
		unBucketNumber[index] = unBucket;
	}
	virtual void SetEntityType(eEntityType EType)
	{
		m_nEntityType = EType;
	}

protected:
	// Model* m_model

	//moved to CRObject base class
	//
	//DirectX::XMFLOAT4X4 m_XMWorldMatrix;
	//std::string m_szTag;
	unsigned int unBucketNumber[4];
	eEntityType m_nEntityType;
};

#endif // _CENTITY_H_
