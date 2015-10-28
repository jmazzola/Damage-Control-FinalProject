/***********************************************
* Filename:			Entity.cpp
* Date:      		05/12/2015
* Mod. Date:		05/14/2015
* Mod. Initials:	BL
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#include "Entity.h"

using namespace DirectX;

#include "..\Core\StructureHolder.h"

/*****************************************************************
* CEntity():
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
CEntity::CEntity( )
{
	DirectX::XMStoreFloat4x4(&MyWorld.Local, DirectX::XMMatrixIdentity());
	m_szTag = "none";
	unBucketNumber[0] = 0;
	unBucketNumber[1] = 0;
	unBucketNumber[2] = 0;
	unBucketNumber[3] = 0;

	m_nEntityType = eEntityType::eCOUNT;
}

/*****************************************************************
* ~CEntity():
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
CEntity::~CEntity( )
{
	// "I don't approve of this, but I'm doing it under duress."
	//				Sorry Future People,
	//						-Gergar
	delete tCollider;
	tCollider = NULL;
}

void CEntity::Update(float dt)
{
}

void CEntity::Enable(DirectX::XMFLOAT3 SpawnPoint, eEnemyVariant variant)
{
}

void CEntity::Disable( )
{
}

DirectX::XMFLOAT3 CEntity::GetPosition( )
{
	DirectX::XMFLOAT3 myPos;
	//MyWorld.UpdateWorld();
	myPos.x = MyWorld.GetWorld()._41;
	myPos.y = MyWorld.GetWorld()._42;
	myPos.z = MyWorld.GetWorld()._43;

	return myPos;
}
